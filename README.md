# Guess-and-Determine-Rebound-2

This repository contains the search/verify codes and full version for the paper "GD-Rebound: Key Collisions on Reduced AES, Rijndael, and the Impact on AES-GCM".

The verification codes are located in the src_verify folder.



Search codes:

aes-cryptanalysis-cp-xor: The code to search RKDCs for key collision attacks on AES. Run ./test.sh to test the RKDC for 3-round AES-128 in the article.
These are multi-part split RAR compression volumes of the complete package aes-cryptanalysis-cp-xor:

1.Download all 7 split part files and place them in the same folder;

2.Only right-click the first volume aes-cryptanalysis-cp-xor.part01.rar;

3.Use compression software (WinRAR, 7-Zip, Bandizip) to extract;

4.The program will automatically combine all split parts and restore the complete file/folder named aes-cryptanalysis-cp-xor.

cp-differential-cryptanalysis-AfricaCrypt22: The code to search RKDCs for key collision attacks on Rijndael. The RKDCs used in the article is listed in the folder of article_trails.

aes-cryptanalysis-cp-xor-keycollision.zip：The code to search RKDCs for key collision attacks on AES, code only without solvers. We modify the files S1Basic.mzn, S1Diff.mzn, S1XOR.mzn and Step2Normal.java to search related-key differentials for key collision attacks.
