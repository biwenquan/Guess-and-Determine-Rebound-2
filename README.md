# Guess-and-Determine-Rebound-2

This repository contains the search/verify codes and full version for the paper "GD-Rebound: Key Collisions on Reduced AES, Rijndael, and the Impact on AES-GCM".

Compile each C file with gcc, and run it:

aes128-2.c: search key collisions for 2-round aes128;
aes128-3.c: search key collisions for 3-round aes128;
aes192-5.c: search key collisions for 5-round aes192;
aes256-6.c: search key collisions for 6-round aes256;
Rijndael-256-256-3.c: search key collisions for 3-round Rijndael-256-256;
aes128-5-semi-free.c: search key collisions for 5-round aes128 in semi-free-target setting;
aes192-7-semi-free.c: search key collisions for 7-round aes192 in semi-free-target setting;
Rijndael-256-256-5-semi-free.c: search key collisions for 5-round Rijndael-256-256 in semi-free-target setting;
aes128-3-GCM.c: practical  key-committing attack on padding fixed AES-GCM with 3-round aes128;
aes192-5-GCM.c: practical  key-committing attack on padding fixed AES-GCM with 5-round aes192;
aes256-6-GCM.c: practical  key-committing attack on padding fixed AES-GCM with 6-round aes256.
