del *.sis
makesis -dc:\symbian\S60_3rd_FP1 podcatcher_udeb.pkg
signsis podcatcher_udeb.sis podcatcher_3rd_udeb_signed.sis podcast.cer podcast.key

makesis -dc:\symbian\S60_3rd_FP1 podcatcher_urel.pkg
signsis podcatcher_urel.sis podcatcher_urel_signed.sis podcast.cer podcast.key