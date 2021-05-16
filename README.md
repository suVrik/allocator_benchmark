# Allocator Benchmark

For proper benchmark source must be compiled in release mode.
The fact that one allocator allocates faster than another doesn't necessary mean that it's better for general purpose allocations.
Each allocator has its own scope of application.

| Allocator | Size | Count | Alloc time | Dealloc time | Alloc improvment* | Dealloc improvment* |
|-|-|-|-|-|-|-|
| New+delete | 8 | 16384 | 1401400us | 377400us | 1 | 1 |
| Buddy | 8 | 16384 | 69200us | 76000us | 20.2514 | 4.96579 |
| Paged pool | 8 | 16384 | 11900us | 7800us | 117.765 | 48.3846 |
| New+delete | 16 | 8192 | 314200us | 196700us | 1 | 1 |
| Buddy | 16 | 8192 | 33900us | 36800us | 9.26844 | 5.34511 |
| Paged pool | 16 | 8192 | 5200us | 3800us | 60.4231 | 51.7632 |
| New+delete | 64 | 4096 | 229300us | 140800us | 1 | 1 |
| Buddy | 64 | 4096 | 20500us | 18500us | 11.1854 | 7.61081 |
| Paged pool | 64 | 4096 | 4200us | 1900us | 54.5952 | 74.1053 |
| New+delete | 256 | 2048 | 300200us | 112700us | 1 | 1 |
| Buddy | 256 | 2048 | 7900us | 9300us | 38 | 12.1183 |
| Paged pool | 256 | 2048 | 2100us | 1000us | 142.952 | 112.7 |
| New+delete | 1024 | 2048 | 747100us | 252400us | 1 | 1 |
| Buddy | 1024 | 2048 | 8300us | 9400us | 90.012 | 26.8511 |
| Paged pool | 1024 | 2048 | 2100us | 1000us | 355.762 | 252.4 |
| New+delete | 4096 | 1024 | 1076400us | 392400us | 1 | 1 |
| Buddy | 4096 | 1024 | 3400us | 4800us | 316.588 | 81.75 |
| Paged pool | 4096 | 1024 | 700us | 500us | 1537.71 | 784.8 |
| New+delete | 16384 | 1024 | 2314900us | 847200us | 1 | 1 |
| Buddy | 16384 | 1024 | 4000us | 4900us | 578.725 | 172.898 |
| Paged pool | 16384 | 1024 | 800us | 600us | 2893.62 | 1412 |
| New+delete | 65536 | 512 | 1415900us | 836000us | 1 | 1 |
| Buddy | 65536 | 512 | 2000us | 2400us | 707.95 | 348.333 |
| Paged pool | 65536 | 512 | 600us | 300us | 2359.83 | 2786.67 |
| New+delete | 262144 | 512 | 1743100us | 1647400us | 1 | 1 |
| Buddy | 262144 | 512 | 2000us | 2500us | 871.55 | 658.96 |
| Paged pool | 262144 | 512 | 500us | 300us | 3486.2 | 5491.33 |
| New+delete | 1048576 | 256 | 1237200us | 1008100us | 1 | 1 |
| Buddy | 1048576 | 256 | 1300us | 1300us | 951.692 | 775.462 |
| Paged pool | 1048576 | 256 | 200us | 200us | 6186 | 5040.5 |
| New+delete | 4194304 | 256 | 1268900us | 948800us | 1 | 1 |
| Buddy | 4194304 | 256 | 1200us | 1400us | 1057.42 | 677.714 |
| Paged pool | 4194304 | 256 | 300us | 100us | 4229.67 | 9488 |

*\* over new+delete of the same size and count.*
