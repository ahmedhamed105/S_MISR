
// protocole name

NAME= Fintrax / CB2A Authorization

// protocol encoding definition

MTI_Enc= PBCD
BMP_Enc= NONE
LEN_Enc= BIGEND
NUM_Enc= PBCD
CHR_Enc= EBCDIC
BIN_Enc= NONE
TRK_Enc= PACKED

// data elements definition
// field number, type, size, size len, subfield, description

FIELD=   0;    B;  16; 0; N;
FIELD=   1;    B;  16; 0; N;
FIELD=   2;    N;  19; 1; N; Primary Account Number
FIELD=   3;    N;   6; 0; N; Processing code
FIELD=   4;    N;  12; 0; N; Transaction Amount
FIELD=   6;    N;  12; 0; N; Amount, cardholder billing
FIELD=   7;    N;  10; 0; N; Transmission date & time
FIELD=  10;    N;   8; 0; N; Conversion rate, cardholder
FIELD=  11;    N;   6; 0; N; Systems trace audit number
FIELD=  12;    N;   6; 0; N; Time, Local transaction
FIELD=  13;    N;   4; 0; N; Date, Local transaction
FIELD=  14;    N;   4; 0; N; Date, Expiration
FIELD=  15;    N;   4; 0; N; Date, Settlement
FIELD=  18;    N;   4; 0; N; Merchant's type
FIELD=  19;    N;   3; 0; N; Acquiring institution country code
FIELD=  22;    N;   3; 0; N; Point of service entry mode
FIELD=  23;    N;   3; 0; N; Card Sequence Number
FIELD=  25;    N;   2; 0; N; Point of service condition code
FIELD=  26;    N;   2; 0; N; Point of service personnal identification number capture code
FIELD=  27;    N;   1; 0; N; Autorisation identification response length
FIELD=  32;    N;  11; 1; N; Acquiring institution identification code
FIELD=  33;    N;  11; 1; N; Forwarding institution identification code
FIELD=  35;    Z;  37; 1; N; Track 2 data
FIELD=  37;   AN;  12; 0; N; Retrieval reference number
FIELD=  38;   AN;   6; 0; N; Authorization identification response
FIELD=  39;   AN;   2; 0; N; Response code
FIELD=  40;   AN;   3; 0; N; Service restriction code
FIELD=  41;  ANS;   8; 0; N; Card acceptor terminal identification
FIELD=  42;  ANS;  15; 0; N; Card acceptor identification code
FIELD=  43;  ANS;  40; 0; N; Card acceptor name/location
FIELD=  44;  ANS;  25; 1; Y; Additionnal response data
FIELD=  47;  ANS; 255; 1; Y; Additional data - national
FIELD=  49;    N;   3; 0; N; Currency code, transaction
FIELD=  51;    N;   3; 0; Y; Currency code, cardholder billing
FIELD=  52;    B;   8; 0; N; Personnal Identification Number (PIN) data
FIELD=  53;    N;  16; 0; N; Security related control information
FIELD=  55;    B; 255; 1; Y; Integrated circuit card system related data
FIELD=  58;  ANS; 255; 1; N; Reserved for national use
FIELD=  59;    B; 255; 1; Y; Reserved for national use
FIELD=  70;    N;   3; 0; N; Network management information code
FIELD=  90;    N;  42; 0; Y; Original data elements
FIELD=  95;   AN;  42; 0; Y; Replacement amounts
FIELD= 100;    N;  11; 0; N; Receiving institution identification code

// message definition
// MTI, mandatory et optional masks, description

MSG=0100;723C448100C088000000000000000000;000000208C2200200000000000000000;CB2A 1.2 Authorization request
MSG=0110;7220000102C088000000000000000000;001E44808C3200200000000000000000;CB2A 1.2 Authorization request response
MSG=9100;7678448100C0A8000000000000000000;00042220AC2202200000000000000000;Fintrax Authorization request
MSG=9110;7220000102C088000000000000000000;045E46808C3222200000000000000000;Fintrax Authorization request response
MSG=9999;72380000000000000000000000000000;844666E1AFF2BA600400004210000000;Any (test) message

// DLL and function name for parsing subfields

SUBDLL= SubFintraxAuth.dll; SubElements; FieldInfoTip