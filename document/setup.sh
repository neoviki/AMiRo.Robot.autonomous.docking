rm -rf ~/AMIRO
#[ Download Amiro OS ]
mkdir ~/AMIRO
cd ~/AMIRO
git clone http://openresearch.cit-ec.de/git/amiro-os.amiro-os.git amiro-os
cd amiro-os
git checkout 1.0_stable


#[ Download ChiBIOS ]

cd ~/AMIRO
git clone https://github.com/ChibiOS/ChibiOS.git ChibiOS
cd ChibiOS
git checkout 2e6dfc7364e7551483922ea6afbaea8f3501ab0e
cp -r ~/AMIRO/amiro-os/patches/* ~/AMIRO/ChibiOS/
for i in `ls | grep patch`; do git am --ignore-space-change --ignore-whitespace < ${i}; done

[ Download Amiro-BLT ]

cd ~/AMIRO
git clone http://openresearch.cit-ec.de/git/amiro-os.amiro-os.git amiro-blt

#[ Swith Branch of Amiro OS ]

cd ~/AMIRO
cd amiro-os
git checkout master
# ./setup.sh [ Project Setup and Compiler Setup]
#git checkout 1.0_stable


