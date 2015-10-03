intgcd_iter(int u, int v) {
  int t;
  while (v) {
    t = u; 
    u = v; 
    v = t % v;
  }
  return u &lt; 0 ? -u : u; /* abs(u) */
}
