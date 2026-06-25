# Sharding
## Introduction
Sharding is a mechanism that allows you to scale out Goblin clusters.
One cluster has capacity limitation. If you want more capacity, you need more clusters with a sharding strategy.

Currently, we support static sharding, which means the number of clusters is determined when creating Goblin clusters.
Clusters can't be scaled out dynamically after creation. We will support dynamic sharding in the future.

## Dynamic Sharding (Design)
For a detailed walkthrough of the existing migration code and the design for dynamic sharding (changing
the shard/cluster size and migrating data live without impacting availability), including HTML diagrams of
the routing gate, consistent-hash rebalance, export/import pipeline, migration state machine and the
end-to-end flow, see [dynamic_sharding_design.html](./dynamic_sharding_design.html).
