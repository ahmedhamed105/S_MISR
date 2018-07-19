
// protocole name

NAME= Amex Authorization

// protocol encoding definition

MTI_Enc= CHAR
BMP_Enc= NONE
LEN_Enc= CHAR
NUM_Enc= CHAR
CHR_Enc= EBCDIC
BIN_Enc= NONE
TRK_Enc= CHAR

// data elements definition
// field number, type, size, size len, subfield, description

FIELD=   0;    B;  16; 0; N;
FIELD=   1;    B;  16; 0; N;
FIELD=   2;    N;  19; 2; N; Primary Account Number
FIELD=   3;    N;   6; 0; N; Processing code
FIELD=   4;    N;  12; 0; N; Transaction amount
FIELD=   7;    N;  10; 0; N; Transmission date & time
FIELD=  11;    N;   6; 0; N; Systems trace audit number
FIELD=  12;    N;  12; 0; N; Transaction local date & time
FIELD=  13;    N;   4; 0; N; Date, Effective
FIELD=  14;    N;   4; 0; N; Date, Expiration
FIELD=  15;    N;   6; 0; N; Date, Settlement
FIELD=  19;    N;   3; 0; N; Acquiring institution country code
FIELD=  22;   AN;  12; 0; N; Point of service data code
FIELD=  24;    N;   3; 0; N; Function code
FIELD=  25;    N;   4; 0; N; Message reason code
FIELD=  26;    N;   4; 0; N; Point of service personnal identification number capture code
FIELD=  27;    N;   1; 0; N; Approval code length
FIELD=  30;    N;  24; 0; N; Original Amount
FIELD=  31;  ANS;  48; 2; N; Acquirer reference data
FIELD=  32;    N;  11; 2; N; Acquiring institution identification code
FIELD=  33;    N;  11; 2; N; Forwarding institution identification code
FIELD=  35;    Z;  37; 2; N; Track 2 data
FIELD=  37;  ANS;  12; 0; N; Retrieval reference number
FIELD=  38;  ANS;   6; 0; N; Approval code
FIELD=  39;    N;   3; 0; N; Action code
FIELD=  41;  ANS;   8; 0; N; Card acceptor terminal identification
FIELD=  42;  ANS;  15; 0; N; Card acceptor identification code
FIELD=  43;  ANS;  99; 2; N; Card acceptor name/location
FIELD=  44;  ANS;  25; 2; Y; Additionnal response data
FIELD=  45;  ANS;  76; 2; N; Track 1 data
FIELD=  47;  ANS; 287; 3; Y; Additional data - national
FIELD=  48;  ANS;  40; 2; N; Additional data - private
FIELD=  49;    N;   3; 0; N; Currency code, transaction
FIELD=  52;    B;   8; 0; N; Personnal Identification Number (PIN) data
FIELD=  53;   AN;   8; 1; N; Security related control information
FIELD=  54;  ANS; 120; 3; N; Additional amounts
FIELD=  55;    B; 256; 3; Y; Integrated circuit card system related data
FIELD=  56;    N;  35; 2; Y; Original data elements
FIELD=  60;  ANS; 300; 3; Y; Reserved for national use
FIELD=  61;  ANS; 100; 3; Y; Reserved for national use
FIELD=  62;  ANS;  60; 2; Y; Reserved for private use
FIELD=  63;  ANS; 205; 3; Y; Reserved for private use

// message definition
// MTI, mandatory et optional masks, description

MSG=1100;703024C0004080000000000000000000;020C0121A8AB1A0E0000000000000000;Authorization request
MSG=1110;60300002024080000000000000000000;120200050C90061E0000000000000000;Authorization response
MSG=1420;703024C0004081000000000000000000;00040003888000000000000000000000;Reversal advice request
MSG=1430;70300002024080000000000000000000;00000001088000000000000000000000;Reversal advice response
