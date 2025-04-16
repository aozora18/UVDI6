BEGIN { 
  RS="\n"
  DRAW=0 
  GBRFN="" 
  APTONLY=0
  SUBFONLY=0
  SUBFI=0
  SUBFIL=0
  GBRIDX=1000
  CUSTIDX=11000
  CUSTINC=1000
  DADD=0
  CUSTGRID=10000
  FORMAT=3.4
  ISENG = 0
  PATHDELIM=""
  if(index(APTFN,"\\") != 0 )
    PATHDELIM = "\\"
  else
    PATHDELIM = "/"
}


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

function handlePOEX(DATA,TYPE) {
  # --- handle a POEX or POIN parameter entry ---
  II=split(DATA,ARY,",")
  for (AI=1; AI<=II; AI++) {
    JJ=index(ARY[AI],"/")
    if ( JJ != 0 ) {
      JJ--
      DC=substr(ARY[AI],1,JJ)
    } else
      DC=ARY[AI]
    DC=DC+DADD
    print "D" DC, "0 d", TYPE, "0 0" >> APTFN
  } # for 
} # handlePOEX 

function checkPARAM(KEY,DATA) {
  # --- check keyword and data of a PARAMETER block entry ---
  if ( APTONLY == 2 ) {
    if ( KEY == "MODE" ) {
      if ( match(DATA,/^I/) == 0 )
        print "ABSOLUTE on" >> APTFN
      else
        print "ABSOLUTE off" >> APTFN
    } else
    if ( KEY == "UNIT" ) {
      if ( match(DATA,/^M/) == 0 ) {
        print "APTUNITS inch" >> APTFN
        ISENG = 1
      }
      else
        print "APTUNITS mm" >> APTFN
    } else
    if ( KEY == "ZERO" ) {
      if ( match(DATA,/^L/) != 0 )
        print "SUPPRESS LEADING" >> APTFN
      else
      if ( match(DATA,/^T/) != 0 )
        print "SUPPRESS TRAILING" >> APTFN
    } else
    if ( KEY == "FORM" ) {
      FORMAT=DATA
      print "FORMAT", DATA >> APTFN
      FDEC=DATA
      sub(/^[0-9]\./,"",FDEC)
      if ( FDEC == "" ) FDEC=0
      CUSTGRID=1
      while( FDEC > 0 ) {
        CUSTGRID *= 10
        FDEC--
      } # while
    } else
    if ( KEY == "IMTP" ) {
      if ( SUBFONLY == 0 ) {
        if ( match(DATA,/^N/) == 0 ) {
          if ( DRAW != 1 )
            DRAW=0
        } else
          DRAW=2
      } # if
    } else
    if ( KEY == "UNIT" ) {
      if ( match(DATA,/^M/) == 0 )
        print "APTUNITS inch" >> APTFN
      else
        print "APTUNITS mm" >> APTFN
    } else
    if (( KEY == "MRGE" ) || ( KEY == "MERGE" )) {
      if ( SUBFONLY == 0 ) {
        if ( match(DATA,/^S/) == 0 ) {
          if ( DRAW != 2 )
            DRAW=0
        } else
          DRAW=1
      } # if
    } # if
  } # if
  if ( KEY == "POEX" ) {
    handlePOEX(DATA,"Poex")
  } else
  if ( KEY == "POIN" ) {
    handlePOEX(DATA,"Poin")
  } else
  if ( KEY == "SUBF" ) {
    if ( SUBFONLY == 0 ) {
      if ( split(DATA,SUBFD,",") >= 3 ) {
        if ( match(SUBFD[3],/^["`'].*["`']$/) != 0 ) {
          LEN=length(SUBFD[3])
          LEN -= 2
          CUSTIDX=CUSTIDX+CUSTINC
          TMPFN=substr(SUBFD[3],2,LEN)
  
          SUBFN[SUBFIL]=TMPFN " " CUSTIDX " ( T" SUBFD[2] " )"
          if(match(TMPFN,/[:\\\/]/) == 0)
            TMPFN = joinPath(DATAPATH,TMPFN)
          else
          {
            GLRET = getline < TMPFN
            close(TMPFN)
            if(GLRET == -1)
            {
              FEXT = ""
              if(match(TMPFN,/\..*$/) != 0)
                FEXT = substr(TMPFN,RSTART,RLENGTH)
              TMPFN = getbasename(TMPFN)
              TMPFN = joinPath(DATAPATH,TMPFN)
              if(length(FEXT))
                TMPFN = TMPFN FEXT
            }
          }
          SUBFNOUT="\"" TMPFN "\" " CUSTIDX " ( T" SUBFD[2] " )"
          SUBFDCODE=SUBFD[1] + (GBRIDX*10)
          print "D" SUBFD[1], "0 f", "Custom" >> APTFN
          print "A" SUBFDCODE, "0 f", "Custom", SUBFNOUT >> APTFN
          CUSTFPATH = DATAPATH
          SUBFIL++
        } # if
      } # if
    } else {
      print GBRFN ": nested subfigure/custom/POLygon aperture not supported"
      exit
    } # if
  } else
  if ( KEY == "NEXT" ) {
    if ( SUBFONLY == 0 ) {
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
  } # if
} # checkPARAM

function round_(NUM)
{
  if ((NUM < 1.0e-6) && (NUM > -1.0e-6))
    return 0

  RNDVAL = NUM
  if(ISENG == 1)
    return RNDVAL 
  POS = index(NUM,".")
  if(POS)
  {
    DV = substr(NUM,POS+5,1)
    if(DV >= 5)
      if(NUM > 0)
        RNDVAL = RNDVAL + 0.00005
      else
        RNDVAL = RNDVAL - 0.00005
  }
  RNDVAL = sprintf("%.4f",RNDVAL)
  return RNDVAL 
}

function decodeCustom(PARAM,DENT) {
  if ( SUBFONLY != 0 ) {
    print GBRFN ": nested subfigure/custom/POLygon aperture not supported"
    exit
  } # if

  CUSTFN=getbasename(GBRFN)
  sub(/\.[^\.]*$/,".",CUSTFN)
  KEYV = KEY-10
  if((KEYV >= 0) && (KEYV <= 989))
    CUSTFN=CUSTFN "." KEY
  else
  {
    HXSTR = sprintf("%x",KEYV+10)
    CUSTFN=CUSTFN HXSTR
  }
  BASECUSTFN = CUSTFN
  CUSTFN = joinPath(APTPATH,CUSTFN)
  PCODE=10
  print "G04%PAR.%*" > CUSTFN
  print "G04%MODE=ABSOLUTE;%*" >> CUSTFN
  print "G04%ZERO=LEADING;%*" >> CUSTFN
  print "G04%FORM=" FORMAT ";%*" >> CUSTFN
  for ( DENTI = 1 ; DENTI <= DENT ; DENTI++ ) {
    if ( match(PARAM[DENTI],/POL,/) != 0 ) {
      PCODE=11
      print "G04%POEX=10;%*" >> CUSTFN
      break
    } else
    if ( match(PARAM[DENTI],/LNE,/) != 0 ) {
      XYLEN=split(PARAM[DENTI],POLDATA,",")
      if ( POLDATA[2] != 0 ) {
        PCODE=11
        print "G04%POEX=10;%*" >> CUSTFN
        break
      } # if
    } # if
  } # for
  print "G04%EOP.%*" >> CUSTFN
  if ( GRIDTENS == 1000 )
    print "G04%APR.%*" >> CUSTFN
  else
    print "G04%APR," GRIDTENS ".%*" >> CUSTFN
  DENTI=0
  ACODE=PCODE
  while( DENTI < DENT ) {
    DENTI++
    TBUFF=PARAM[DENTI]
    if ( match(TBUFF,/^[+-]*REL.*/) != 0 ) {
      sub(/REL/,"CIR",TBUFF)
      gsub(/,X.*/,"",TBUFF)
    } else 
    if ( match(TBUFF,/^[+-]*LNE.*/) != 0 ) {
      XYLEN=split(TBUFF,POLDATA,",")
      if ( POLDATA[2] == 0 ) {
        sub(/LNE/,"CIR",TBUFF)
        gsub(/,X.*/,"",TBUFF)
      } else {
        continue
      } # if
    } else
    if ( match(TBUFF,/^[+-]*POL.*/) != 0 ) {
      continue
    } else {
      if (( match(TBUFF,/^[+-]*ARC.*/) != 0 ) ||
          ( match(TBUFF,/^[+-]*REA.*/) != 0 )) {
        XYBUFF=TBUFF
        sub(/,X.*/,"",TBUFF)
        sub(/^[^X]*,X/,"X",XYBUFF)
        XYLEN=split(XYBUFF,POLDATA,",")
        if ( XYLEN < 4 ) {
          XYBUFF="X0Y0X0Y0"
        } else {
          sub(/X/,"",POLDATA[1])
          sub(/Y/,"",POLDATA[2])
          sub(/X/,"",POLDATA[3])
          sub(/Y/,"",POLDATA[4])
          TBUFF=TBUFF ",X0,Y0,X" POLDATA[3]-POLDATA[1] ",Y" POLDATA[4]-POLDATA[2]
        } # if 
      } else {
        sub(/,X.*/,"",TBUFF)
      } # if
    } # if
    print "G04%A" ACODE ":" TBUFF ".%*" >> CUSTFN
    ACODE++
  } # while
  print "G04%EOA.%*" >> CUSTFN
  DENTI=0
  ACODE=PCODE
  while( DENTI < DENT ) {
    DENTI++
    TBUFF=PARAM[DENTI]
    if ( match(TBUFF,/^[+-]*REL.*/) != 0 ) {
      sub(/^.*REL,/,"",TBUFF)
      gsub(/[XY]/,"",TBUFF)
      XYLEN=split(TBUFF,POLDATA,",")
      X0=POLDATA[2] * CUSTGRID / GRIDTENS
      Y0=POLDATA[3] * CUSTGRID / GRIDTENS
      print "D" ACODE "*X" X0 "Y" Y0 "D02*" >> CUSTFN
      X0=POLDATA[4] * CUSTGRID / GRIDTENS
      Y0=POLDATA[5] * CUSTGRID / GRIDTENS
      print "X" X0 "Y" Y0 "D01*" >> CUSTFN
      ACODE++
    } else 
    if ( match(TBUFF,/^[+-]*LNE.*/) != 0 ) {
      sub(/^.*LNE,/,"",TBUFF)
      gsub(/[XY]/,"",TBUFF)
      XYLEN=split(TBUFF,POLDATA,",")
      if ( POLDATA[2] == 0 ) {
        X0=POLDATA[2] * CUSTGRID / GRIDTENS
        Y0=POLDATA[3] * CUSTGRID / GRIDTENS
        print "D" ACODE "*X" X0 "Y" Y0 "D02*" >> CUSTFN
        X0=POLDATA[4] * CUSTGRID / GRIDTENS
        Y0=POLDATA[5] * CUSTGRID / GRIDTENS
        print "X" X0 "Y" Y0 "D01*" >> CUSTFN
        ACODE++
      } else { # non-zero width LNE
        XLEN=POLDATA[4]-POLDATA[2]
        YLEN=POLDATA[5]-POLDATA[3]
        NORM=sqrt(XLEN*XLEN + YLEN*YLEN)
        HALFW=POLDATA[1]/2*CUSTGRID/GRIDTENS
        XLEN=XLEN*HALFW
        YLEN=YLEN*HALFW
        XPT0=(POLDATA[2]-YLEN/NORM) * CUSTGRID / GRIDTENS
        YPT0=(POLDATA[3]+XLEN/NORM) * CUSTGRID / GRIDTENS
        print "D10*X" XPT0 "Y" YPT0 "D02*" >> CUSTFN
        XPT1=(POLDATA[4]-YLEN/NORM) * CUSTGRID / GRIDTENS
        YPT1=(POLDATA[5]+XLEN/NORM) * CUSTGRID / GRIDTENS
        print "X" XPT1 "Y" YPT1 "D01*" >> CUSTFN
        XPT1=(POLDATA[4]+YLEN/NORM) * CUSTGRID / GRIDTENS
        YPT1=(POLDATA[5]-XLEN/NORM) * CUSTGRID / GRIDTENS
        print "X" XPT1 "Y" YPT1 "D01*" >> CUSTFN
        XPT1=(POLDATA[2]+YLEN/NORM) * CUSTGRID / GRIDTENS
        YPT1=(POLDATA[3]-XLEN/NORM) * CUSTGRID / GRIDTENS
        print "X" XPT1 "Y" YPT1 "D01*" >> CUSTFN
        print "X" XPT0 "Y" YPT0 "D01*" >> CUSTFN
      } # if
    } else 
    if ( match(TBUFF,/^[+-]*POL.*/) == 0 ) {
      if (( index(TBUFF,"X") != 0 ) && ( index(TBUFF,"Y") != 0 )) {
        XSTR=TBUFF
        sub(/^[^X]*,X/,"",XSTR)
        YSTR=XSTR
        sub(/^[^Y]*,Y/,"",YSTR)
        gsub(/,Y.*$/,"",XSTR)
        XSTR=XSTR * CUSTGRID / GRIDTENS
        YSTR=YSTR * CUSTGRID / GRIDTENS
        TBUFF="X" XSTR "Y" YSTR
      } else
        TBUFF="X0Y0"
      print "D" ACODE "*" TBUFF "D03*" >> CUSTFN
      ACODE++
    } else { # Polygons
      PENT=split(TBUFF,POLDATA,",")
      if ( PENT >= 9 ) { # at least 3 vertices
        X0=POLDATA[2]
        sub(/X/,"",X0)
        X0 = X0 * CUSTGRID / GRIDTENS
        Y0=POLDATA[3]
        sub(/Y/,"",Y0)
        Y0 = Y0 * CUSTGRID / GRIDTENS
        print "D10X" X0 "Y" Y0 "D02*" >> CUSTFN
        XSTR=X0
        YSTR=Y0
        PENTI=4
        while( PENTI <= PENT ) {
          if ( POLDATA[PENTI] == "CUR" ) {
            PENTI++
            ANGSTR=POLDATA[PENTI]
            ANGSTR = ANGSTR * 3.14159265358 / 648000000
            SINA=sin(ANGSTR)
            COSA=cos(ANGSTR)
            X1=XSTR*COSA - YSTR*SINA
            if ( X1 < 0 )
              X1 -= 0.5
            else
              X1 += 0.5
            sub(/\..*/,"",X1)
            Y1=XSTR*SINA + YSTR*COSA
            if ( Y1 < 0 )
              Y1 -= 0.5
            else
              Y1 += 0.5
            sub(/\..*/,"",Y1)
            PENTI++
            XSTR=POLDATA[PENTI]
            sub(/X/,"",XSTR)
            XSTR=XSTR * CUSTGRID / GRIDTENS
            XSTR += X1
            PENTI++
            YSTR=POLDATA[PENTI]
            sub(/Y/,"",YSTR)
            YSTR=YSTR * CUSTGRID / GRIDTENS
            YSTR += Y1
            print "G75G03X" XSTR "Y" YSTR "I" X1 "J" Y1 "D01*G74*" >> CUSTFN
          } else { # POLDATA[PENTI] == "STR"
            PENTI++
            XSTR=POLDATA[PENTI]
            sub(/X/,"",XSTR)
            XSTR=XSTR * CUSTGRID / GRIDTENS
            PENTI++
            YSTR=POLDATA[PENTI]
            sub(/Y/,"",YSTR)
            YSTR=YSTR * CUSTGRID / GRIDTENS
            print "X" XSTR "Y" YSTR "D01*" >> CUSTFN
          } #if
          PENTI++
        } # while
        if ( (X0 != XSTR) || (Y0 != YSTR) )
          print "X" X0 "Y" Y0 "D01*" >> CUSTFN
      } # if PENT >= 9
    } # if Polygons
  } # while
  print "M00*" >> CUSTFN
  close(CUSTFN)
  CUSTIDX=CUSTIDX+CUSTINC
  SUBFN[SUBFIL]=BASECUSTFN " " CUSTIDX
  SUBFNOUT = "\"" CUSTFN "\" " CUSTIDX
  DKEY=KEY+(GBRIDX*10)
  print "D" KEY, "0 f", "Custom" >> APTFN
  print "A" DKEY, "0 f", "Custom", SUBFNOUT >> APTFN
  CUSTFPATH = APTPATH
  SUBFIL++
} # decodeCustom

function decodePrimitive(PRIM) {
  IENT=split(PRIM,ENTRIES,",")
  # --- ENTRIES has the parameter list of the definition ---
  if ( IENT >= 2 ) {
    TYPE=ENTRIES[1]
    sub(/^[+-]/,"",TYPE)
    if ( TYPE == "POL" ) {
      ENTRIES[1]=PRIM
      decodeCustom(ENTRIES,1)
    } else {
      RAD=ENTRIES[2]/GRID
      DKEY=KEY+DADD
      printf "D%s %lg d ",DKEY,round_(RAD) >> APTFN
      if ( TYPE == "CIR" )
        printf "Round %s\n", round_(RAD) >> APTFN
      else
      if ( TYPE == "SQR" )
        printf "Square %s\n", round_(RAD) >> APTFN
      else
      if ( TYPE == "REC" ) {
        if ( IENT < 3 )
          YY=RAD
        else
          YY=ENTRIES[3]/GRID
        printf "Rect %s %s\n", round_(RAD), round_(YY) >> APTFN
      } else
      if ( TYPE == "LNE" ) {
        if ( ENTRIES[3] == ENTRIES[5] ) { # vertical rectangle
          XX=RAD
          Y0=ENTRIES[4]
          sub(/Y/,"",Y0)
          Y1=ENTRIES[6]
          sub(/Y/,"",Y1)
          if ( Y0 > Y1 )
            YY=Y0-Y1
          else
            YY=Y1-Y0
          YY=YY/GRID
          printf "Rect %s %s\n", round_(XX), round_(YY) >> APTFN
        } else
        if ( ENTRIES[4] == ENTRIES[6] ) { # horizontal rectangle
          YY=RAD
          X0=ENTRIES[3]
          sub(/X/,"",X0)
          X1=ENTRIES[5]
          sub(/X/,"",X1)
          if ( X0 > X1 )
            XX=X0-X1
          else
            XX=X1-X0
          XX=XX/GRID
          printf "Rect %s %s\n", round_(XX), round_(YY) >> APTFN
        } else {
          print GBRFN, "D" KEY ": non-orthogonal LNE not supported"   
          exit
        } # if
      } else
      if ( TYPE == "REL" ) {
        if ( ENTRIES[3] == ENTRIES[5] ) { # vertical oblong
          XX=RAD
          Y0=ENTRIES[4]
          sub(/Y/,"",Y0)
          Y1=ENTRIES[6]
          sub(/Y/,"",Y1)
          if ( Y0 > Y1 )
            YY=Y0-Y1
          else
            YY=Y1-Y0
          YY=YY/GRID
          printf "Oblong %s %s\n", round_(XX), round_(YY) >> APTFN
        } else
        if ( ENTRIES[4] == ENTRIES[6] ) { # horizontal oblong
          YY=RAD/GRID
          X0=ENTRIES[3]
          sub(/X/,"",X0)
          X1=ENTRIES[5]
          sub(/X/,"",X1)
          if ( X0 > X1 )
            XX=X0-X1
          else
            XX=X1-X0
          XX=XX/GRID
          printf "Oblong %s %s\n", round_(XX), round_(YY) >> APTFN
        } else {
          print GBRFN, "D" KEY ": non-orthogonal REL not supported"   
          exit
        } # if
      } else
      if ( TYPE == "REA" ) {
        RAD=ENTRIES[2]
        if ( match(ENTRIES[3],/X/) == 0 ) { # angle given 
          if ( ENTRIES[3] != 1296000000 ) { # non-Donut
            print GBRFN, "D" KEY ": only 360 degree REA is supported"
            exit
          } else { # full circle
            X0=ENTRIES[6]
            Y0=ENTRIES[7]
          } # if
        } else { # full circle
          X0=ENTRIES[5]
          Y0=ENTRIES[6]
        } # if
        sub(/X/,"",X0)
        sub(/Y/,"",Y0)
        YY=X0*X0+Y0*Y0
        YY=sqrt(YY)*2
        XX=(YY+RAD)/GRID
        YY=(YY-RAD)/GRID
        printf "Donut %s %s\n", round_(XX), round_(YY) >> APTFN
      } else {
        print GBRFN, "D" KEY ": " TYPE, "tool not supported"
        exit
      } # if
    } # if
  } # if
} # decodePrimitive

function checkDCODE(BUFF) {
  # --- check and output 1 DCode definition
  if ( match(BUFF,/^A[0-9]+:/) != 0 ) {
    split(BUFF,PARAM,":")
    # --- PARAM[1] has the DCode, PARAM[2] has the definition ---
    KEY=PARAM[1]
    sub(/^A0*/,"",KEY)
    if ( KEY == "" ) KEY=0
    KKEY=KEY-10
    if (( KKEY >= 0 ) && ( KKEY <= 9999 )) { # DCode range 10-10009
      DEFN=PARAM[2]
      DENT=split(DEFN,PARAM,";")
      # --- PARAM has simple primitives ---
      if ( DENT > 1 )
        decodeCustom(PARAM,DENT)
      else
        decodePrimitive(PARAM[1])
    } # if
  } # if
} # checkDCODE

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

    BUFF=GBRFN
    gsub(/\n/,"",BUFF)
    gsub(/\r/,"",BUFF)
    if ( APTONLY == 0 ) { # form apt file name from 1st MDA file name
      APTONLY=2
      print "" > APTFN
      APTPATH=APTFN
      LEN=match(APTPATH,/[^\\/]*$/)
      LEN--
      APTPATH=substr(APTPATH,1,LEN)
      print "CUSTOMDIR",APTPATH > APTFN
    } # if
    DATAPATH=GBRFN
    GBRFN=BUFF            
    LEN=match(DATAPATH,/[^\\/]*$/)
    LEN--
    DATAPATH=substr(DATAPATH,1,LEN)
    RS="*"

    while( GBRFN != "" ) {
      MODE=0
      RETVAL=getline < GBRFN
      if ( RETVAL != 1 ) {
        GBRFN=substr(GBRFN,DALEN+1,256)
        GBRFN=tolower(GBRFN)
        GBRFN=DATAPATH GBRFN
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
              if ( BUFF == "PAR." ) {
                MODE=1
              } # if
            } else
            if ( MODE == 1 ) { # PARAMETER DATA BLOCK end with %EOP.%
              if ( BUFF == "EOP." )
                MODE=2
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
            } else
            if ( MODE == 2 ) { # wait until %APR.% block
              if ( match(BUFF,/^APR/) != 0 ) {
                RUNBUFF=""
                MODE=3
                GRID=BUFF
                sub(/^APR[,\.]/,"",GRID)
                if ( GRID=="" ) GRID=1000
                else sub(/\.$/,"",GRID)
                GRIDTENS = GRID
                if (ISENG == 0)
                  GRID = GRID *  0.03937007874016
              } # if
            } else
            if ( MODE == 3 ) { # APERTURE DEFINITION block
              if ( BUFF == "EOA." ) {
                break
              } else {
                RUNBUFF=RUNBUFF BUFF
                I=index(RUNBUFF,".")
                if ( I != 0 ) {
                  J=I-1
                  BUFF=substr(RUNBUFF,1,J)
                  J=I+1
                  RUNBUFF=substr(RUNBUFF,J)
                  # --- Now BUFF contains 1 aperture definition ---
                  checkDCODE(BUFF)
                } # if
              } # if
            } # if
          } # if
        } # if
        RETVAL=getline < GBRFN
      } # while
      close(GBRFN)
      if ( SUBFI < SUBFIL ) {
        if ( split(SUBFN[SUBFI],TWONM," ") > 1 )
          DADD=TWONM[2]
        else
          DADD=0

        CPTH=TWONM[1]
        LEN=match(CPTH,/[^\\/]*$/)
        LEN--
        CPTH=substr(CPTH,1,LEN)
        if(length(CPTH))
        {
          TMPFN = TWONM[1]
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
          GBRFN=joinPath(CUSTFPATH,TWONM[1])

        SUBFI++
        SUBFONLY=1
      } else
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
          GBRFN=joinPath(DATAPATH,NEXTFN)
        NEXTFN=""
        SUBFONLY=0
        SUBFI=0
        SUBFIL=0
        GBRIDX=GBRIDX + 1000
        DADD=0
      } else
        GBRFN=""
      APTONLY=3
    } # while

    NEXTFN=""
    SUBFONLY=0
    SUBFI=0
    SUBFIL=0
    GBRIDX=GBRIDX + 1000
    DADD=0

    RS="\n"
    RETVAL=getline < INPFN
  } # while
} # main

END {
  close(APTFN)
}# END
