/*  _________________________________________________________________________________________________

                                 Driver LMX2325               
   _________________________________________________________________________________________________
    Name: Driver LMX2325
    Author: Juan Maureira Llanos
    Date: 20/03/08 23:11
    Description: Driver para manejar PLL LMX2325 de National Semiconductors, 
                 diseñado para frecuencias desde 88.1 hasta 107.9. 
    _________________________________________________________________________________________________           
   REGISTRO R                                                                   
    __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ 
   |23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
                   __ __ __ __                                           __
   |<-No usados-->| X| X| X| S|<----------------R counter-------------->| C|
                |<------------------Bits enviados----------------------->|
   _________________________________________________________________________________________________
                                                                                                     
   bit 15 Select preescaler -> S = 0; preescaler 64/65
                        ->   S = 1; preescaler 32/33
                                                                                                     
   bit  0 Control bit       -> C = 0; Los datos son transferidos al N counter
                      -> C = 1; Los datos son transferidos al R counter     
   _________________________________________________________________________________________________                         
                       S                         C      
    __ __ __ __ __ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   |23|22|21|20|19|X|X|X|1|0|0|0|0|0|0|0|0|1|0|1|0|0|0|1| => R = 0x08051
               |<----------Se envia el 8051--------->|
   Primero se envia al 19-bit Data Register, el 40 + control bit C  = 1 + S = 1                   
   _________________________________________________________________________________________________

   REGISTRO N                                                                                       
    __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ 
   |23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|   
   |--No usados---|<-----------B counter---------->|<-----A counter---->| C|
               |<------------------Bits enviados----------------------->|

   Fvco = [(P*B)+A]*Fosc/R;               A < B
   _________________________________________________________________________________________________

   R_counter
        X X X|S|<-----------R------------->|C       
         _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _       
         |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
        0 0 0 1 0 0 0 0 0 0 0 0 1 0 1 0 0 0 1       
   
   _________________________________________________________________________________________________

   N_counter

     Dial|<--------B---------->|<-----A----->|C  
    ____ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _       
   |____|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
    881 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 0 0 0 1|0
    882 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 0 0 1 0|0
    883 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 0 0 1 1|0
    884 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 0 1 0 0|0    
    885 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 0 1 0 1|0
    886 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 0 1 1 0|0 
    887 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 0 0 0|0 
    889 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 0 0 1|0 
    890 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 0 0 1|0  
     891 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 0 1 0|0 
    892 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 1 0 0|0 
    893 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 1 0 1|0
    894 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 1 1 0|0 
    895 |0 0 0 0 0 0 1 1 0 1 1|0 0 1 1 1 1 1|0 
    896 |0 0 0 0 0 0 1 1 1 0 0|1 0 0 0 0 0 0|0
        .
     .
        .
    1010|0 0 0 0 0 0 1 1 1 1 1|0 0 1 0 0 1 0|0  
    1011|0 0 0 0 0 0 1 1 1 1 1|0 0 1 0 0 1 1|0 
    1012|0 0 0 0 0 0 1 1 1 1 1|0 0 1 0 1 0 0|0 
    1013|0 0 0 0 0 0 1 1 1 1 1|0 0 1 0 1 0 1|0 
    1014|0 0 0 0 0 0 1 1 1 1 1|
    1015|0 0 0 0 0 0 1 1 1 1 1|
    1016|0 0 0 0 0 0 1 1 1 1 1|
    1017|0 0 0 0 0 0 1 1 1 1 1|
     .
     .
     .
    1069|
    1070|
    1071|            
 
   _________________________________________________________________________________________________
   
                                    Funciones    
   _________________________________________________________________________________________________
   
   void LoadPll(c[3]);                     Envia los bits hacia el PLL
   
   int32 frecuencia(int16 canal);            Carga en el PLL el nuevo canal

   void pll_init(void);                  Inicializa PLL cargando el R counter             
   _________________________________________________________________________________________________
                                    
*/

//  _________________________________________________________________________________________________
# ifndef   LE
# define   LE      PIN_C0                  // Pines que comandan al PLL
# define   DATA   PIN_C1                  //
# define   CLOCK   PIN_C2                  //
# endif
//  _________________________________________________________________________________________________
boolean R_flag;
//  _________________________________________________________________________________________________
void LoadPll(c[3]){
   
BYTE i;
   
   for(i=1;i<=(24-19);i++)                   // Ya que el rango de buffer[3] es de 24 bits y, el pll se carga con solo 19 bits, 
      shift_left(c,3,0);                     // desplazo los bits que me sobran, (24-19 = 5 bits).
      
   output_low(CLOCK);                        // y espero con el clock en bajo
   delay_us(2);                              // 2 useg
   
   for(i=1;i<=19;i++){                       // Bucle para enviar los 19 bits al pll
      output_bit(DATA,shift_left(c,3,0));    // Comenzando desde el MSB
      delay_us(20);                          // espero 20 useg
      output_high(CLOCK);                    // Envio señal de clock
      delay_us(10);
      output_low(CLOCK);
      delay_us(10);      
   }
   
   output_high(LE);
   delay_us(10);
   output_low(LE);
   delay_us(10);
}      
//  _________________________________________________________________________________________________
void pll_init(void){         
   
   int32 var_1 = 0x00008051;                     
   int32 var_2 = 0x000080A1;               
   int32 R_counter;                      
   byte buffer[3];                        

      if(!R_flag)                        // Si R_flag = 1 cargo R_counter con 0x00008051
         R_counter = var_1;               // Frec_ref = 100Khz

      if(R_flag)                        // Si R_flag = 0 cargo R_counter con 0x000080A1      
         R_counter = var_2;               // Frec_ref = 50Khz

      R_counter &= 0x0000FFFF;
               
      buffer[0] = (byte)(R_counter);         // Guardo 1er byte desde LSB en buffer[0]
      buffer[1] = (byte)(R_counter >> 8);      // Guardo 2o byte en buffer[1]
      buffer[2] = (byte)(R_counter >> 16);   // Guardo 3er byte en buffer[2]

      LoadPll(buffer);                  // Cargo R en pll
}
//  _________________________________________________________________________________________________
int32 frecuencia(int16 canal){
            //Para frecuencias de FM (88.1-107.1) canal debe estar entre 1 y 190
   byte   buffer[3];                     // Variables locales en Ram
   int32 temp_N_counter;                  //
   int32 temp;                           //
   byte   A_counter;                     // 
   int32 temp_dial;   
                                    //                         ej: para canal = 1 -> Frec = temp_N_counter = 881;               
   temp_N_counter = 881 + (canal-1);            // guarda nueva frecuencia ej: dial = 1 => canal = 1 -> temp_N_counter = 881;

   R_flag = 0;   
         
   temp = temp_N_counter / 32;                  // calcula B (parte entera)ej: temp = 881 / 32 = 27,53125 => temp = 27; B = 00000011011

   A_counter = temp_N_counter - (temp * 32);    // Calcula A             ej: A_counter = 881 - (27*32) = 17; A = 0010001;

   temp_dial =    temp_N_counter;
   
     if(temp <  A_counter){                  // Si temp (B_counter) <= A_counter 
      
      R_flag = 1;

      temp_N_counter /= 0.5;
      
        temp = temp_N_counter / 32;             

      temp_dial = temp_N_counter;

      temp_dial *= 0.5;                        
 
       A_counter = temp_N_counter - (temp * 32);// y calculo nuevo A_counter
     }

   pll_init();
 
   temp = (temp << 7) | A_counter;              // Desplaza 7 bits a la 
                                    //   izq, y mete los 7 bits del A_counter                
   temp = (temp << 1) | 0;                  // Desplazo un bit a la    ej: temp = 0000001101100100010;
                                    // izq y le meto el C = 0 
                                    // para que lo cargue en pll ,temp es ahora N_counter
   // guarda N counter en 3 bytes
   buffer[0] = (byte)(temp);                    // Guardo 1er byte desde LSB en buffer[0]
   buffer[1] = (byte)(temp >> 8);            // Guardo 2o byte en buffer[1]
   buffer[2] = (byte)(temp >> 16);            // Guardo 3er byte en buffer[2]
   
   LoadPll(buffer);                             // carga N counter          ej: N = 0000001101100100010;

   return(temp_dial);                     // Retorno el dial FM
}
//  _________________________________________________________________________________________________