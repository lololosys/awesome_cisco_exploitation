/* $Id: alert_tool.l,v 3.1 1995/11/09 13:13:57 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/snanm/alert_tool.l,v $
 *------------------------------------------------------------------
 * This is a lex file tool to convert the alert template to header file
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: alert_tool.l,v $
 * Revision 3.1  1995/11/09  13:13:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:03:45  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

%s NAME VEC ID TYPE DESCR CAUSES CODES ACTS SUBFS FIELD
 /* Whitespace - blank tab or newline */
W [" "\n\t]
 /* Separator - one or more whitespace or 
 //             a comma plus 0 or more  whitespace */
S {W}+|({W}*\,{W}*)
 /* Identifier - alpha plus 0 or more alphanumeric */
I [a-zA-Z][a-zA-Z0-9]*
 /* Hex char */
H [a-fA-F0-9]
 /* Actions introducer (ACT=)*/
A (A|a)(C|c)(T|t){W}*=
 /* Detail subfields introducer (DET=)*/
D (D|d)(E|e)(T|t){W}*=
%{
#define uchar unsigned char;
 /* vars */
 
 /* The four kinds of cause must occur in this order.
 // "Add" is shown as a fifth "cause".  This allows the same code to
 // handle the additional subvectors after the cause subvectors. */
 enum Cause { Prob, User, Install, Fail, Add};
 enum Cause cause;
  int hexVal (char hex);
  char hexChar (int hex) ;
  void CrcByte(char * byt, int index);
  void CrcDelimiter(void);
  void AppendInit (char * s);
  long int CalculateCRCAlertId(unsigned char *message, int len);
  void StartSubVec(char * name, char * key) ;
  void StartSubVecNoKey(char * name) ;
  void EndSubVec(void) ;
  void EndMajVec(void) ;
  void StartActions(void) ;
  void EndSubVecAndInsertDateTimeSubVec(void);

 typedef enum{FALSE, TRUE}boolean;
 
 int   sfNum;                 /* for making unique subfield names */
 int   fixNum;                /* for making unique names for fixed parts of subfields */
 int   code;                  /* for making unique names for cause and action codes */
 int   codesLevel;            /* snapshots value of 'level' when codes started */
 char* causeName = "puif";    /* Prob, User, Install, Fail */
 char* causeVal = "3456";     /* 93    94    95       96   */
 boolean causeSubfs[4] = {FALSE,TRUE,TRUE,TRUE};
 int  level;                  /* nesting level of vectors, 0 is major vector */
 int  svLenPos[9];            /* offset in initVal at which to write length */
 int  svLen[9];               /* length value to write */
 char initVal [10000];        /* array of init data */
 int  initValCount;           /* length of init data */
 int  idPos;                  /* for inserting the alertId when using crc algorithm*/
 boolean calcId = FALSE;      /* flag to indicate if the alertId is to be calculated */
 int crcInArrayPos = 0;       /* Index into array input to the crc algorithm to find alert id */
 unsigned char crcArray[100]; /* array input into crc algorithm to find alert id */
 int  typePos;                /* offset in initVal to write type */
 char id   [9];               /* alert id */
 char fieldName [99];         /* named field */
 boolean foundCause = FALSE;  /* to determine if cause or action */
 char defineVar[20];

 
 char * preamble0 = 
   "\n { unsigned char A_altLenHi, A_altLenLo, A_altKeyHi, A_altKeyLo;\n";
 char * preamble1 = 
   "  unsigned char A_genFlags0, A_genFlags1, A_type, A_descrHi, A_descrLo, A_id0, A_id1, A_id2, A_id3;\n";
 char * comment = 
   "\n/*\n * This file is machine generated \n * Do not attempt to edit it or place it under CVS control\n */\n\n";

%}
%%
    fprintf (yyout, "%s", comment);
{W}*\/\/.* {
  /* Don't pass //-style comments through */
};

(##) {BEGIN NAME;
   /* printf("// BEG NAME;\n"); */
   sfNum = 0;
   fixNum = 0;
   level = 0; 
};

<NAME>{I} {BEGIN VEC;
    /* printf("// BEG VEC;\n"); */
    level = 0;
    svLen[level] = 4;        /* init maj vec len */
    strcpy (initVal, yytext);
    strcpy (defineVar, yytext);
    initValCount = yyleng;
    AppendInit ("={0x");
    svLenPos[level] = initValCount;  /* pos to fill in maj vec len at end */
    AppendInit ("??,0x??,0,0");
    fprintf (yyout, "%s", preamble0);
    StartSubVec ("gen", "92");
    AppendInit (",0,0,0x");
    typePos = initValCount;
    AppendInit ("??,0x??,0x??");
    fprintf (yyout, "%s", preamble1);
    svLen[level] += 5;
};

<VEC>{W}*\{{W}* {BEGIN ID;
  /* printf("// BEG ID;\n"); */
};

<ID>{H}{8,8}    {
    /* AlertId supplied */
    char i;
    strcpy (id, yytext);
    for (i = 0; i < 8;) {
      AppendInit (",0x");
      initVal[initValCount++] = id[i++];
      initVal[initValCount++] = id[i++];
      svLen[level]++;
    }
    EndSubVecAndInsertDateTimeSubVec();  
};

<ID>\? {
    /* AlertId not supplied */
    idPos = initValCount;     /* save position in initVal to insert AlertId */
    AppendInit (",0x??,0x??,0x??,0x??");
    svLen[level] += 4;
    calcId = TRUE;
    EndSubVecAndInsertDateTimeSubVec();  
};

<ID>{W}*\,{W}*  {BEGIN TYPE;
  /* printf("// BEG TYPE;\n"); */
};

<TYPE>{H}{2,2}  {
    initVal[typePos] = yytext[0];
    initVal[++typePos] = yytext[1];
    if (calcId)  
        CrcByte(yytext, 1);
};

<TYPE>{W}*\,{W}* {BEGIN DESCR;
  /* printf("// BEG DESCR;\n"); */
};

<DESCR>{H}{4,4}  {
    typePos += 4;                 /* reuse as descrPos */
    initVal[typePos] = yytext[0];
    initVal[++typePos] = yytext[1];
    typePos += 4;
    initVal[typePos] = yytext[2];
    initVal[++typePos] = yytext[3];
    if (calcId)
        CrcByte(yytext, 2);
};

<DESCR>{W}*\,  {BEGIN CAUSES;
  /* printf("// BEG CAUSES;\n"); */
                cause = Prob;
};

<CAUSES>{W}*\,{W}*   {
  if (++cause <= Fail) { 
      if (calcId)
          CrcDelimiter();
  }
  else if (cause > Fail) {
    codesLevel = level;
BEGIN SUBFS;
  /* printf("// BEG SUBFS(Add);\n"); */
  }

};

<CAUSES>{W}*\{{W}*   {
                  char cos[3];
                  char val[3];
   foundCause = TRUE;
BEGIN CODES;
  /* printf("// BEG CODES;\n"); */
                  strcpy(cos, "cx");
                  cos [1] = causeName[cause];
                  strcpy (val, "9x");
                  val [1] = causeVal[cause];
                  StartSubVec(cos, val);
                  code = 0;
                  
                  if (causeSubfs[cause]) {
                      cos[0]= 'f';
                      StartSubVec(cos,"01");
                  }
};

<CODES,ACTS,SUBFS>{S} ;

<CODES,ACTS>{W}*\}{W}*    {  EndSubVec(); /* End the 96 subvector */


BEGIN CAUSES;
  /* printf("// BEG CAUSES;\n"); */
                             if (causeSubfs[cause])
                                 EndSubVec();
};

<CODES>{D}           {  codesLevel = level;
                        if (causeSubfs[cause])
                            EndSubVec();
                        else {
                            printf("\n*** Illegal actions in subvector ***");
                            return;
                        }

BEGIN SUBFS;
  /* printf("// BEG SUBFS;\n"); */
};

<ACTS>{D}           {  EndSubVec();
                       codesLevel = level;

BEGIN SUBFS;
  /* printf("// BEG SUBFS;\n"); */
};

<SUBFS>{W}*\{{W}*    {
                  char sf[5];
                  sprintf (sf, "d%d", sfNum++);
                  StartSubVecNoKey(sf);
};

<SUBFS>{A} {
                   if (level > codesLevel)
                     printf ("\n*** Illegal actions inside subvector\n");
                   else
                     StartActions();
};

<CODES>{A} {       if (causeSubfs[cause]) {
                       EndSubVec();
                       StartActions();
                   }
                   else {
                       printf("\n*** Illegal actions in subvector ***");
                       return;
                   } 
};   

<SUBFS>{W}*\}{W}*    { 
                       if (level <= codesLevel) {
                         if (cause == Add) {
                           /* End of additional vectors */
                           EndMajVec();
                         } else {
BEGIN CAUSES;
  /* printf("// BEG CAUSES;\n"); */

                           EndSubVec();  /* end codes subvec */
                         }
                       } else 
                         EndSubVec();  /* end nested subvec */
};

<CAUSES>{W}*\}{W}*    {BEGIN NAME;
  /* printf("// BEG NAME;\n"); */
};

<CODES,ACTS>{H}{4,4}  {
                 fprintf (yyout, "  unsigned char A_c%cc%dHi, A_c%cc%dLo;\n", 
                 causeName[cause], code, causeName[cause], code);
                 code++;

                 AppendInit (",0x??,0x??");
                 initVal[initValCount-7] = yytext[0];
                 initVal[initValCount-6] = yytext[1];
                 initVal[initValCount-2] = yytext[2];
                 initVal[initValCount-1] = yytext[3];
                 svLen[level] += 2;

                 if (calcId && foundCause)
                     CrcByte(yytext, 2);

};


<SUBFS>{I}\:  {
                 yytext[yyleng-1] = 0; /* remove colon */
                 fprintf (yyout, "  unsigned char %s", yytext);
                 if (yyleng > 98) {
                   printf ("\n*** Field name %s too long", yytext);
                 } else {
                   strcpy (fieldName, yytext);
                 }
BEGIN FIELD;
  /* printf("// BEG FIELD;"); */
};

<FIELD>{H}+ {
                  fprintf (yyout, " [0x%s];\n", yytext);
                  if (yyleng > 2) {
                    printf ("\n*** Field %s too long", fieldName);
                  } else {
                    int fldLeng = 0;
                    int ind;
                    for (ind = 0; ind < yyleng; ind++) {
                      fldLeng = fldLeng*16 + hexVal(yytext[ind]);
                    }
                    svLen [level] += fldLeng;
                    AppendInit(",{");
                    for (; fldLeng > 0; fldLeng--) {
                      AppendInit ("0");
                      (fldLeng == 1) ? AppendInit("}") : AppendInit(",") ;
                    }
                  }
BEGIN SUBFS;
  /* printf("// BEG SUBFS;\n"); */
};

<SUBFS>{H}+  {
              int byt = yyleng/2;
              if (byt*2 < yyleng)
                printf ("\n*** Odd number of hex digits\n");
              else {
                char * strPtr;
                fprintf(yyout, "  unsigned char A_f%d[%d];\n", fixNum++, byt);
                svLen [level] += byt;
                AppendInit (",{");
                for (strPtr = yytext; byt; byt--) {
                  AppendInit ("0x");
                  initVal[initValCount++] = *strPtr++;
                  initVal[initValCount++] = *strPtr++;
                  (byt==1) ? AppendInit("}") : AppendInit(",") ;
                }
              }
}

%%


    
  int hexVal (char hex) 
  { if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
  }
  
  char hexChar (int hex) 
  { if (hex >= 0x0 && hex <= 0x9) return hex + '0';
    if (hex >= 0xa && hex <= 0xf) return hex - 10 + 'a';
  }

  void CrcByte(char * byt, int index)
  {
    int i;
    for (i=0; i<=index; i+=2)
      crcArray[crcInArrayPos++] = hexVal(byt[i])*0x10 + hexVal(byt[i+1]);
  }

  void CrcDelimiter(void)
  {
    crcArray[crcInArrayPos++] = 0xFF;
    crcArray[crcInArrayPos++] = 0xFF;    
  }

  void AppendInit (char * s) {
    /* append string to initVal and update length */
    strcpy (initVal+initValCount, s);
    initValCount += strlen(s);
  }
  
 /* If no AlertId was supplied then this function will calculate
  * the AlertId given a string of Alert Information in the
  * following order :
  *   AlertType 
  *   AlertDescription 
  *   ProbableCauses 
  *   FFFF 
  *   UserCauses 
  *   FFFF 
  *   InstallCauses
  *   FFFF
  *   FailureCauses
  */ 
 long int CalculateCRCAlertId(unsigned char *message, int len)
  { 
    int i,j;
    boolean carry;
    int nextMessageByte;
    int nextCrcBit;
    long int crc;
    long int generatorPoly = 0x04c11db7;  /* 26,23,22,16,12,11,8,7,5,4,2,1,0 */

    /*
     * The CRC algorithm requires the message be changed as follows :
     *     1. append the message with 0x00000000
     *     2. XOR the first four bytes of the message with 0xFFFFFFFF
     */

    crc = (message[3] + message[2]*0x100 + message[1]*0x10000
          + message[0]*0x1000000) ^ 0xFFFFFFFF;    

    for (i=4; i<(len+4); i++)
    {
        nextMessageByte = (i < len) ? message[i] : 0x00;   
        for (j=0; j<8; j++)
	{
            nextCrcBit = (nextMessageByte & 0x80) ? 1 : 0;
            nextMessageByte <<= 1;
            carry = crc & 0x80000000;
            crc = (crc << 1) + nextCrcBit;
            if (carry) crc ^= generatorPoly;    
	}
    }    

    /*
     * Now, the Alert ID is the complement of the remainder
     * polynomial.
     */
    crc = ~crc;

    return crc;
 }


  void StartSubVec(char * name, char * key) {
    /* generate subvec header and start accumulating length at new level */
    fprintf (yyout, "  unsigned char A_%sLen, A_%sKey;\n", name, name);
    AppendInit ("\n,0x");
    svLenPos[++level] = initValCount;
    svLen[level] = 2;
    sprintf (initVal+initValCount, "??,0x%s", key);
    initValCount += 7;
  };
  
  void StartSubVecNoKey(char * name) {
    /* same as StartSubVec but key is in following fixed data */
    fprintf (yyout, "  unsigned char A_%sLen;\n", name);
    AppendInit ("\n,0x??");
    svLenPos[++level] = initValCount-2;
    svLen[level] = 1;
  };
  
  void EndSubVec() {
    /* fill in subvec length, unwind one level and accumulate length 
    // in containing vector */
    char vecLenHex [5];
    sprintf (vecLenHex, "%0.2X", svLen[level]);
    /* printf("//Vector length = %s\n", vecLenHex); */
    initVal[svLenPos[level]] = vecLenHex[0];
    initVal[svLenPos[level]+1] = vecLenHex[1];
    svLen[level-1] += svLen[level];
    level --;
  };

  void EndSubVecAndInsertDateTimeSubVec() {

    EndSubVec();  /* end of generic alert */

    /* insert date/time subvector */
    StartSubVec ("rel", "42");
    fprintf (yyout, "  unsigned char A_reltime[5];\n");
    AppendInit (",{0,0,0,0,0}");
    svLen[level] += 5;
    EndSubVec();
  };

  void EndMajVec() {
    char vecLenHex [5];
    long int id;
    int i;    

    /* fill in maj vec len */
    sprintf (vecLenHex, "%0.4X", svLen[level]);
    /* printf("//Vector length = %s\n", vecLenHex); */
    initVal[svLenPos[level]] = vecLenHex[0];
    initVal[svLenPos[level]+1] = vecLenHex[1];
    initVal[svLenPos[level]+5] = vecLenHex[2];
    initVal[svLenPos[level]+6] = vecLenHex[3];
    
    /* fill in the alert id if not given*/
    if (calcId)
    {
       id = CalculateCRCAlertId(crcArray, crcInArrayPos);
       initVal[idPos+3] = hexChar((id >> 28) & 0xF);
       initVal[idPos+4] = hexChar((id >> 24) & 0xF);
       initVal[idPos+8] = hexChar((id >> 20) & 0xF);
       initVal[idPos+9] = hexChar((id >> 16) & 0xF);
       initVal[idPos+13]= hexChar((id >> 12) & 0xF);
       initVal[idPos+14]= hexChar((id >> 8)  & 0xF);
       initVal[idPos+18]= hexChar((id >> 4)  & 0xF);
       initVal[idPos+19]= hexChar(id & 0xF); 
       calcId = FALSE;
    }

    /* append initVal to output stream */
    fprintf (yyout, "\n} %s\n};\n", initVal);

    /* add define at end */
    fprintf (yyout,"\n#define %s_H\n",defineVar);

    /* printf("// BEG 0; \n"); */
BEGIN 0;
  };
  
  void StartActions() {
    char cos[3];

BEGIN ACTS;
    foundCause = FALSE;  
    /* printf("// BEG ACTS; \n"); */
    strcpy (cos, "ax");
    cos [1] = causeName[cause];
    StartSubVec(cos, "81");
  };

main()
{
   yyout = stdout;
   yyin = stdin;
   yylex();
   exit(0);
}











