
// protocole name

NAME= Base24 External ISO (Also Host ISO)

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

FIELD=   2;    N;     19; 2; N; Primary Account Number
FIELD=   3;   AN;      6; 0; N; Processing Code
FIELD=   4;    N;     12; 0; N; Transaction Amount
FIELD=   7;    N;     10; 0; N; Transmission date & time
FIELD=  11;    N;      6; 0; N; Systems trace audit number
FIELD=  12;    N;      6; 0; N; Local transaction time
FIELD=  13;    N;      4; 0; N; Local transaction date
FIELD=  14;    N;      4; 0; N; Date expiration
FIELD=  15;    N;      4; 0; N; Settlement date
FIELD=  17;    N;      4; 0; N; Capture date
FIELD=  18;    N;      4; 0; N; Merchant type
FIELD=  19;    N;      3; 0; N; Acquiring institution country code
FIELD=  22;    N;      3; 0; N; Point of service entry mode
FIELD=  23;    N;      3; 0; N; Card sequence number
FIELD=  25;    N;      2; 0; N; POS condition code
FIELD=  27;    N;      1; 0; N; Auth ID response length
FIELD=  28;   AN;      9; 0; N; Transaction Fee Amount
FIELD=  32;   AN;     11; 2; N; Acquiring institution identification code
FIELD=  33;    N;     11; 2; N; Forwarding institution identification code
FIELD=  34;   AN;     28; 2; N; Extended PAN
FIELD=  35;    Z;     37; 2; N; Track 2 data
FIELD=  37;   AN;     12; 0; N; Retrieval reference number
FIELD=  38;   AN;      6; 0; N; Approval code
FIELD=  39;   AN;      2; 0; N; Action code
FIELD=  41;  ANS;     16; 0; N; Card acceptor terminal identification
FIELD=  42;  ANS;     15; 0; N; Card acceptor identification code
FIELD=  43;  ANS;     40; 0; N; Card acceptor Name/location
FIELD=  44;  ANS;     27; 2; N; Additional response data
FIELD=  45;  ANS;     76; 2; N; Track 1 data
FIELD=  48;  ANS;    203; 3; Y; Additional data
FIELD=  49;    N;      3; 0; N; Transaction currency code
FIELD=  52;   AN;     16; 0; N; PIN data
FIELD=  53;    N;     16; 0; N; Security related control information
FIELD=  54;  ANS;    123; 3; N; Additional amounts
FIELD=  58;  ANS;    135; 3; N; Financial token
FIELD=  59;  ANS;    135; 3; N; CAF update token
FIELD=  60;  ANS;    200; 3; N; Terminal data/FHM data
FIELD=  61;  ANS;    200; 3; N; Card issuer data/FHM user data
FIELD=  62;  ANS;    200; 3; N; From Host maintenance CAF exponent
FIELD=  63;  ANS;    200; 3; N; POS tokens/ATM PIN offset/FHM enh. preauth data
FIELD=  64;   AN;     16; 0; N; Primary MAC
FIELD=  70;    N;      3; 0; N; Network Management info code
FIELD=  73;    N;      6; 0; N; Action date (YYMMDD)
FIELD=  90;    N;     42; 0; N; Original data elements
FIELD=  91;   AN;      1; 0; N; FHM File update code (1-Add, 2-Rpl, 3-Del, 5-Inq, 9-Inc)
FIELD=  95;   AN;     42; 0; N; Replacement amounts
FIELD=  98;  ANS;     25; 0; N; Payee
FIELD= 100;    N;     11; 2; N; Receiving institution ID
FIELD= 101;  ANS;     32; 2; N; FHM File name (CC-PBF, CF-CAF,...)
FIELD= 102;  ANS;     28; 2; N; FHM Account ID1
FIELD= 103;  ANS;     28; 2; N; FHM Account ID2
FIELD= 104;  ANS;    100; 3; N; Transaction description
FIELD= 112;  ANS;    105; 3; N; FHM enhanced pre-auth hold info
FIELD= 113;  ANS;    276; 3; N; FHM Hot card update info
FIELD= 114;  ANS;    645; 3; N; FHM PBF Customer segment info
FIELD= 115;  ANS;    643; 3; N; FHM CAF/PBF base user info
FIELD= 116;  ANS;    389; 3; N; FHM CAF non-currency dispense
FIELD= 117;  ANS;    153; 3; N; FHM CAF EMV
FIELD= 118;  ANS;    103; 3; N; FHM CAF and PBF data
FIELD= 119;  ANS;    200; 3; N; FHM Self-Service banking check info
FIELD= 120;  ANS;    117; 3; N; FHM CAF/PBF Expanded segment
FIELD= 121;  ANS;    200; 3; N; FHM CAF Expanded ATM segment
FIELD= 122;  ANS;    200; 3; N; FHM CAF/PBF Expanded POS segment
FIELD= 123;  ANS;    375; 3; N; POS invoice data/ATM deposit amt/FHM CAF card segment
FIELD= 124;  ANS;    663; 3; N; FHM CAF/PBF Preauth segment
FIELD= 125;  ANS;    276; 3; N; POS settlement data/ATM acc ind/FHM app file
FIELD= 126;  ANS;    693; 3; N; FHM CAF expanded account/PBF cred line segment
FIELD= 127;  ANS;    200; 3; N; POS user data/FHM CAF address segment
FIELD= 128;   AN;     16; 0; N; Secondary MAC

// message definition
// MTI, mandatory et optional masks, description

MSG=0100;B238840128A18018000000001000009C0000000000000000;800600200040100200000000000000200000000000000000;Authorization Request
MSG=0110;B23880012E81801800000000100000880000000000000000;800604000040000200000000040000440000000000000000;Authorization Response
MSG=0420;B23880012EA0801800000040100000000000000000000000;8002000000400402000000020600007C0000000000000000;Reversal Advice
MSG=0430;B22000012A80800000000040000000000000000000000000;800000000000000000000002060000040000000000000000;Reversal Advice Response
MSG=0300;C22000000001801000000020080000000000000000000000;800000000000000400000000040029D40000000000000000;FHM request
MSG=0310;C22000000201000000000020080000000000000000000000;800000000000801400000000040029D40000000000000000;FHM response
MSG=0800;822000000000000004000000000000000000000000000000;800000000000080000000000000001200000000000000000;Network Management Request
MSG=0810;822000000200000004000000000000000000000000000000;800000000000080000000000000001200000000000000000;Network Management Response
MSG=0200;3238800128A0801000000000000000000000000000000000;800206000041140A000000001C0000140000000000000000;Financial Request
MSG=0210;323880012EA0801000000000000000000000000000000000;800200000050040A000000001C0000140000000000000000;Financial Response
