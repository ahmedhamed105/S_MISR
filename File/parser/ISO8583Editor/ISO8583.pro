
// protocole name

NAME= Base ISO8583 (2003) example

// protocol encoding definition

MTI_Enc= CHAR
BMP_Enc= HEX
LEN_Enc= CHAR
NUM_Enc= CHAR
CHR_Enc= ASCII
BIN_Enc= NONE
TRK_Enc= CHAR

// data elements definition
// field number, type, size, size len, subfield, description

FIELD=   0;    B;  16; 0; N;
FIELD=   1;    B;  16; 0; N;
FIELD=   2;    N;  19; 2; N; Primary Account Number
FIELD=   3;   AN;   6; 0; N; Processing Code
FIELD=   4;    N;  16; 0; Y; Transaction Amount
FIELD=   7;    N;  10; 0; N; Transmission date & time
FIELD=  11;    N;  12; 0; N; Systems trace audit number
FIELD=  12;    N;  14; 0; Y; Date and time local transaction
FIELD=  13;    N;   6; 0; N; Date effective
FIELD=  14;    N;   4; 0; N; Date expiration
FIELD=  18;    B; 140; 3; Y; Message error indicator
FIELD=  19;    N;   3; 0; N; Acquiring institution country code
FIELD=  22;    B;  16; 0; N; Point of service data code
FIELD=  23;    N;   3; 0; N; Card sequence number
FIELD=  25;    N;   4; 0; N; Message reason code
FIELD=  26;    N;   4; 0; N; Merchant category code
FIELD=  27;    B;  27; 0; Y; POS capability
FIELD=  31;    N;  11; 2; N; Acquiring institution identification code
FIELD=  32;    N;  11; 2; N; Acquiring institution identification code
FIELD=  33;    N;  11; 2; N; Forwarding institution identification code
FIELD=  35;    Z;  37; 2; N; Track 2 data
FIELD=  37;   AN;  12; 0; N; Retrieval reference number
FIELD=  38;   AN;   6; 0; N; Approval code
FIELD=  39;    N;   4; 0; N; Action code
FIELD=  40;    N;   3; 0; N; Service code
FIELD=  41;  ANS;  16; 0; N; Card acceptor terminal identification
FIELD=  42;  ANS;  35; 2; N; Card acceptor identification code
FIELD=  53;    B;  48; 2; N; Security related control information
FIELD=  58;    N;  11; 2; N; Authorizing agent institution identification code

// message definition
// MTI, mandatory et optional masks, description

MSG=0100;72300000000000000000000000000000;000C0661A9C000000000000000000000;Authorization request
MSG=0110;72300001020000000000000000000000;0000400084C000400000000000000000;Authorization request response
MSG=0420;72300081000000000000000000000000;0000000080C000400000000000000000;Reversal advice
MSG=0440;72300081020000000000000000000000;0000000080C000400000000000000000;Reversal notification
MSG=0450;72340000000000000000000000000000;00000000000000000000000000000000;Reversal notification acknowledgement
