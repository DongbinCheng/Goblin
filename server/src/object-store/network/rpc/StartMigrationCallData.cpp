/************************************************************************
Copyright 2021-2022 eBay Inc.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**************************************************************************/

#include "StartMigrationCallData.h"

#include "../../ObjectStore.h"

namespace goblin::objectstore::network {

StartMigrationCallData::StartMigrationCallData(
    proto::KVStore::AsyncService *service,
    grpc::ServerCompletionQueue *cq,
    kvengine::KVEngine &kvEngine,
    objectstore::ObjectStore &objectStore)
    : RequestCallData(service, cq, kvEngine, objectStore), mResponder(&mContext) {
  // Invoke the serving logic right away.
  proceed();
}

void StartMigrationCallData::proceed() {
  if (mStatus == CREATE) {
    // Make this instance progress to the PROCESS state.
    mStatus = PROCESS;

    // As part of the initial CREATE state, we *request* that the system
    // start processing SayHello requests. In this request, "this" acts as
    // the tag uniquely identifying the request (so that different CallData
    // instances can serve different requests concurrently), in this case
    // the memory address of this CallData instance.
    mService->RequestStartMigration(&mContext, &mRequest, &mResponder,
                         mCompletionQueue, mCompletionQueue, this);
  } else if (mStatus == PROCESS) {
    // Spawn a new CallData instance to serve new clients while we process
    // the one for this CallData. The instance will deallocate itself as
    // part of its FINISH state.
    new StartMigrationCallData(mService, mCompletionQueue, mKVEngine, mObjectStore);
    auto s = mObjectStore.triggerMigration(mRequest.status(), mRequest.routeinfo(), mRequest.migrationtask());
    proto::StartMigration::Response resp;
    if (s.isNotLeader()) {
      mStatus = FINISH;
      resp.mutable_header()->set_code(proto::ResponseCode::NOT_LEADER);
      mResponder.Finish(resp, grpc::Status::OK, this);
    } else {
      mStatus = FINISH;
      resp.mutable_header()->set_code(proto::ResponseCode::OK);
      mResponder.Finish(resp, grpc::Status::OK, this);
    }
  } else {
    GPR_ASSERT(mStatus == FINISH);
    // Once in the FINISH state, deallocate ourselves (CallData).
    delete this;
  }
}

}  /// namespace goblin::objectstore::network


