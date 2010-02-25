del *.sis
makesis -dc:\symbian\S60_5th_Edition_SDK_v1.0 podcatcher_udeb.pkg
signsis podcatcher_udeb.sis podcatcher_udeb_signed.sis podcast.cer podcast.key

makesis -dc:\symbian\S60_5th_Edition_SDK_v1.0 podcatcher_urel.pkg
signsis podcatcher_urel.sis podcatcher_urel_signed.sis podcast.cer podcast.key