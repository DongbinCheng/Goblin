// Left/Right arrow keys move between demo slides.
document.addEventListener('keydown', function (e) {
  if (e.key === 'ArrowRight') { const n = document.querySelector('a[rel="next"]'); if (n && !n.classList.contains('disabled')) location.href = n.href; }
  if (e.key === 'ArrowLeft')  { const p = document.querySelector('a[rel="prev"]'); if (p && !p.classList.contains('disabled')) location.href = p.href; }
});
