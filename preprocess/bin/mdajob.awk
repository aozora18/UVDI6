BEGIN {
  RS="\n"
  DRAW=0
  DRAWPOS="Paint"
  DRAWCUT="Scratch"
  DRAWNET="Negative"
  DRAWCNG="Paint"
  NEXTFLG=0
  IMTPFLG=0
  MERGFLG=0
  GBRFN=""
  JOBFN=""
  LAYERS=""
  LAYER=1
  COLOR[1]=",255,0,0"
  COLOR[2]=",0,255,0"
  COLOR[3]=",255,255,0"
  COLOR[4]=",0,0,255"
  COLOR[5]=",255,0,255"
  COLOR[6]=",0,255,255"
  PATHDELIM=""
  if(index(APTFN,"\\") != 0 )
    PATHDELIM = "\\"
  else
    PATHDELIM = "/"
} # BEGIN

function joinPath(PTH,FN) {
  if ( index(FN,PATHDELIM) != 0 )
    return FN
  else {
    if ( PTH != PATHDELIM )
      sub(/[\\/]*$/,"",PTH)
    if ( PTH == "." )
      PTH=""
    if ( PTH == "" )
      FNAME=FN
    else
      FNAME=PTH PATHDELIM FN
    return FNAME
  } # if
} # joinPath

function getbasename(FULLFNAME)
{
  DRLEN = split(FULLFNAME,DRARR,/[\/\\]/)
  if (DRLEN == 0 )
    BNAME = GBRFN
  else
  {
    BNAME = DRARR[DRLEN]
    BNLEN = split(BNAME,BNARR,".")
    if(BNLEN)
      BNAME = BNARR[1]
  }
  return BNAME
}# getbasename

function checkPARAM(KEY,DATA) {
  # --- check keyword and data of a PARAMETER block entry ---
  if ( KEY == "IMTP" ) {
    if ( match(DATA,/^N/) == 0 ) {
      IMTPFLG=0
    } else
      IMTPFLG=1
  } else
  if ( KEY == "MRGE" ) {
    if ( match(DATA,/^S/) == 0 ) {
      DRAW=0
    } else
      DRAW=1
  } else
  if ( KEY == "NFLG" ) {
    if ( match(DATA,"^P") == 0 )
      NEXTFLG=1
    else
      NEXTFLG=0
  } else
  if ( KEY == "NEXT" ) {
    NEXTFN=DATA
    if (( NEXTFN == "-" ) || ( NEXTFN == "\"-\"" )) {
      NEXTFN=""
    } # if
    if ( match(NEXTFN,/^["`'].*["`']$/) != 0 ) {
      LEN=length(NEXTFN)
      LEN -= 2
      NEXTFN=substr(NEXTFN,2,LEN)
    } # if
  } # if
} # checkPARAM

{ # main


  INPFN = ARGV[1]
  RETVAL=getline < INPFN
  while( RETVAL == 1) {

    BUFF=$0
    GFPARMLEN = split(BUFF,GFPARMARR,",")
    if(GFPARMLEN)
      GBRFN = GFPARMARR[1]
    else
      GBRFN=""

    if ( JOBFN == "" ) {
      JOBFN=APTFN
      sub(/\.apt$/,".job",JOBFN)
      sub(/\.APT$/,".JOB",JOBFN)
      print "" > JOBFN
    } # if

    DATAPATH=APTFN
    LEN=match(DATAPATH,/[^\\/]*$/)
    LEN--
    DATAPATH=substr(DATAPATH,1,LEN)
    DATAPATH2=GBRFN
    LEN=match(DATAPATH2,/[^\\/]*$/)
    LEN--
    DATAPATH2=substr(DATAPATH2,1,LEN)
    RS="*"
    while( GBRFN != "" ) {
      MODE=0
      RETVAL=getline < GBRFN
      if ( RETVAL != 1 ) {
        GBRFN=tolower(GBRFN)
        RETVAL=getline < GBRFN
      } # if
      while(RETVAL == 1) {
        # --- Save quoted string, if any, before stripping ---
        BUFF=$0
        QSTR = ""
        if(match(BUFF,/["'`]/))
        {
          QC = substr(BUFF,RSTART,1)
          if((QC == "\"") && match(BUFF,/["].*["]/))
            QSTR = substr(BUFF,RSTART,RLENGTH)
          else if((QC == "'") && match(BUFF,/['].*[']/))
            QSTR = substr(BUFF,RSTART,RLENGTH)
          else if((QC == "`") && match(BUFF,/[`].*[`]/))
            QSTR = substr(BUFF,RSTART,RLENGTH)
        }
        # --- Strip all white space in the input line ---
        BUFF=$1
        for (I=2; I<=NF; I++)
          BUFF=BUFF $I
        # --- Restore quoted string, if any. ---
        if(QSTR != "")
        {
          if(QC == "\"")
            sub(/["].*["]/,QSTR,BUFF)
          else if(QC == "'")
            sub(/['].*[']/,QSTR,BUFF)
          else if(QC == "`")
            sub(/[`].*[`]/,QSTR,BUFF)
        }
        gsub(/\n/,"",BUFF)
        gsub(/\r/,"",BUFF)
        # --- Data format must be G04%COMMENT% ---
        if ( match(BUFF,"^G0*4%.*%$") != 0 ) {
          sub(/^G0*4%/,"",BUFF)
          sub(/%$/,"",BUFF)
          # --- BUFF now contains the parameter data ---
          if ( BUFF != "" ) {
            if ( MODE == 0 ) { # wait until %PAR.% block
              if ( BUFF == "PAR." ) MODE=1
            } else
            if ( MODE == 1 ) { # PARAMETER DATA BLOCK end with %EOP.%
              if ( BUFF == "EOP." )
                break
              else {
                IENT=split(BUFF,ENTRIES,";")
                for (I=1; I<=IENT; I++) { # --- process 1 entry at a time
                  sub(/==/,"=",ENTRIES[I])
                  J=split(ENTRIES[I],PARAM,"=")
                  if ( J == 2 ) 
                    # --- Now PARAM[1] is the keyword PARAM[2] is the data ---
                    checkPARAM(PARAM[1],PARAM[2])
                } # for
              } # if
            } # if
          } # if
        } # if
        RETVAL=getline < GBRFN
      } # while
      close(GBRFN)
      if ( DRAW == 0 ) {
        MERGFLG = 0
        if ( IMTPFLG == 1 )
          DRAW=DRAWNEG
        else
          DRAW=DRAWPOS
      } else {
        MERGFLG = 1
        if ( IMTPFLG == 1 )
          DRAW=DRAWCNG
        else
          DRAW=DRAWCUT
      } # if
      print "[layer" LAYER "]" >>JOBFN
      GBRFULLFN=GBRFN
      LEN=match(GBRFULLFN,/[^\\/]*$/)
      LEN--
      GBRFULLFN=substr(GBRFULLFN,1,LEN)
      if(length(GBRFULLFN))
        GBRFULLFN = GBRFN
      else
        GBRFULLFN = DATAPATH2 GBRFN

      DRLEN = split(GBRFN,DRARR,/[\/\\]/)
      if (DRLEN == 0 )
        LNAME = GBRFN
      else
      {
        LNAME = DRARR[DRLEN]
        BNLEN = split(LNAME,BNARR,".")
        if(BNLEN)
          LNAME = BNARR[1]
      }
      if(NEXTFLG == 0)
        LNTAG1 = "P"
      else
        LNTAG1 = "M"
      if(MERGFLG == 0)
        LNTAG2 = "P"
      else
        LNTAG2 = "S"
      if(IMTPFLG == 0)
        LNTAG3 = "P"
      else
        LNTAG3 = "N"
      LNTAG = LNTAG1 LNTAG2 LNTAG3

      LNAME = LNAME LNTAG LAYER
      print "layer=" LNAME "," GBRFULLFN >>JOBFN
      LAYCOL=LAYER % 6
      if ( LAYCOL == 0 ) LAYCOL=6
      print "gbrvulopt=ON," DRAW COLOR[LAYCOL] >>JOBFN
      if ( LAYERS != "" ) LAYERS=LAYERS ","
      LAYERS=LAYERS "layer" LAYER
      LAYER++
  
      if ( NEXTFN != "" ) {

        NXTPTH=NEXTFN
        LEN=match(NXTPTH,/[^\\/]*$/)
        LEN--
        NXTPTH=substr(NXTPTH,1,LEN)
        if(length(NXTPTH))
        {
          TMPFN = NEXTFN
          GLRET = getline <TMPFN
          close(TMPFN)
          if(GLRET == -1)
          {
            FEXT = ""
            if(match(TMPFN,/\..*$/) != 0)
              FEXT = substr(TMPFN,RSTART,RLENGTH)
            TMPFN = getbasename(TMPFN)
            TMPFN = joinPath(DATAPATH2,TMPFN)
            if(length(FEXT))
              TMPFN = TMPFN FEXT
          }
          GBRFN=TMPFN
        }
        else
          GBRFN=DATAPATH2 NEXTFN
        NEXTFN=""
        if ( NEXTFLG == 0 ) {
          DRAWPOS="Paint"
          DRAWCUT="Scratch"
          DRAWNET="Negative"
          DRAWCNG="Paint"
          IMTPFLG=0
        } else {
          if ( IMTPFLG == 0 ) {
            DRAWPOS="Paint"
            DRAWCUT="Scratch"
            DRAWNET="Negative"
            DRAWCNG="Paint"
          } else {
            DRAWPOS="Scratch"
            DRAWCUT="Paint"
            DRAWNET="Scratch"
            DRAWCNG="Paint"
          } # if
        } # if
        NEXTFLG=0
        DRAW=0
      } else
        GBRFN=""
      APTONLY=1
    } # while

    RS="\n"
    RETVAL=getline < INPFN
  } # while

} # main

END {
  print "[General]" >>JOBFN
  print "layers=" LAYERS >>JOBFN
  print "aptfile=" APTFN >>JOBFN
} # END
