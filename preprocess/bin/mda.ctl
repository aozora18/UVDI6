T:TMP1
V:OPENCAPTION Open GBR (MDA Format) File
V:OPENEXT gbr
V:OPENFILTER MDA Files (*.GBR)\0*.gbr\0All Files (*.*)\0*.*\0
D:GetFiles MDAFILE $(TMP1) 256 $(OPENCAPTION) $(OPENEXT) $(OPENFILTER)
F:CvtExt APTOUT '$(MDAFILE)' .APT
F:CvtExt JOBOUT '$(MDAFILE)' .JOB
F:SetPath APTOUT '$(APTOUT)'
F:SetPath JOBOUT '$(JOBOUT)'
F:ChgSlashes APTOUT2 '$(APTOUT)'

CW:
X:'$(GAWKPATH)/winawk' -f '$(GAWKPATH)/mda.awk' -v 'APTFN=$(APTOUT2)' '$(TMP1)'
X:'$(GAWKPATH)/winawk' -f '$(GAWKPATH)/mdajob.awk' -v 'APTFN=$(APTOUT2)' '$(TMP1)'
CR:
F:Erase $(TMP1)
