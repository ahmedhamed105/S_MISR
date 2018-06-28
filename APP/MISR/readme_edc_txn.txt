EDC - Credit Card Demo
======================
Supported Transaction Types:
1. Sale (contactless, EMV chip, magnetic swipe, manual keyin)
   - perform at idle screen
2. Void / Offline / Refund / Auth / Adjustment / Settlement Txns
   - select to run by pressing [Down] key
3. Reversal / Batch-Upload Txns
   - internal flow


Supported Functions (select by pressing [Left] key):
01. Display Transaction Batch
02. Display Transaction Total
50. Set Demo EDC
72. Reprint Last Tranasction
74. Print Transaction Total
75. Print Batch Transactions


Steps to Initialize EDC for Demo Purpose:
1. Enter EDC - Func 50 "Set Demo EDC"
   - initialize the basic terminal & EMV parameters (& reboot)
2. Enter APM - Func 61 "Training Mode"
   - Access Code = DLL * (DLL-1) * (DLL - 2)   (e.g. if DLL=12, Access Code = 001320)
   - set Training Mode = Enable (by press YELLOW key to toggle)
   - EDC will not connect to real host to get response in this case
3. Enter EDC - Func 51 "Set Demo CTL"
   - set EMV contactless parameters
4. In EDC idle screen, enter an amount and tap/insert/swipe/keyin the card for transaction
