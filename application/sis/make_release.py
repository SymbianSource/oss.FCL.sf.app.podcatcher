import os;

#del *.sis - delete all sis files python style
os.system('del *.sis');

#update 'build' number in PKG file
os.system('hg log -l 1 > temp.txt');

os.system('makesis -d%EPOCROOT% podcatcher_udeb.pkg');
os.system('signsis podcatcher_udeb.sis podcatcher_udeb_signed.sis podcast.cer podcast.key');

#update 'build' number in PKG file
os.system('makesis -d%EPOCROOT% podcatcher_urel.pkg');
os.system('signsis podcatcher_urel.sis podcatcher_urel_signed.sis podcast.cer podcast.key');