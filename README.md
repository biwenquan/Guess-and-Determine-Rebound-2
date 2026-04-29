# Guess-and-Determine-Rebound-2

This repository contains the search/verify codes and full version for the paper "GD-Rebound: Key Collisions on Reduced AES, Rijndael, and the Impact on AES-GCM".

The verification codes are located in the src_verify/ folder.

The RKDCs search tools codes are located in the search_tools/ folder, these tools need to be extracted into a single file and can be used immediately.

We also provide code without the solver included, with some additional modifications.


cp-differential-cryptanalysis-AfricaCrypt22: The code to search RKDCs for key collision attacks on Rijndael.  We modify the files sat/Rijndael.mzn and src/main/kotlin/fr/limos/decrypt/apps/rijndael/step2/sat/OptimizeForSatKt.kt to search related-key differentials for key collision attacks.

aes-cryptanalysis-cp-xor-keycollision.zip：The code to search RKDCs for key collision attacks on AES. We modify the files S1Basic.mzn, S1Diff.mzn, S1XOR.mzn and Step2Normal.java to search related-key differentials for key collision attacks.
