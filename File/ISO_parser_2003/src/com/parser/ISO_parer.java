/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.parser;

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Properties;
import javax.xml.bind.DatatypeConverter;

/**
 *
 * @author ahmed.elemam
 */
public class ISO_parer extends javax.swing.JFrame {

    
    
    /**
     * Creates new form ISO_parer
     * 0 binary
     * 1 char
     * 2 number
     * 3 alphanumeric characters
     * 4 field containing magnetic track data
     * 5 LL one byte
     * 6 LLL two byte
	 
	 
	 another number 
	 0 Fixed
	 1 .. up to
	 3 dynmaic with lenght byte
     */
    public ISO_parer() {
        initComponents();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jLabel1 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTextArea1 = new javax.swing.JTextArea();
        jLabel2 = new javax.swing.JLabel();
        jButton1 = new javax.swing.JButton();
        jScrollPane2 = new javax.swing.JScrollPane();
        jTextArea2 = new javax.swing.JTextArea();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jLabel1.setText("Bank Miser Parser");

        jTextArea1.setColumns(20);
        jTextArea1.setRows(5);
        jScrollPane1.setViewportView(jTextArea1);

        jLabel2.setText("Msg");

        jButton1.setText("Parse");
        jButton1.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton1MouseClicked(evt);
            }
        });

        jTextArea2.setColumns(20);
        jTextArea2.setLineWrap(true);
        jTextArea2.setRows(5);
        jScrollPane2.setViewportView(jTextArea2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(layout.createSequentialGroup()
                        .addGap(26, 26, 26)
                        .addComponent(jLabel1))
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 527, Short.MAX_VALUE))
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jLabel2))
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jButton1))
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jScrollPane2)))
                .addContainerGap(46, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel1)
                .addGap(14, 14, 14)
                .addComponent(jLabel2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 138, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jButton1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 135, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton1MouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jButton1MouseClicked
        // TODO add your handling code here:
        
        Properties prop = new Properties();
	InputStream input = null;

	try {
            
       String current = new java.io.File( "." ).getCanonicalPath();
        System.out.println("Current dir:"+current);
 String currentDir = System.getProperty("user.dir");
        System.out.println("Current dir using System:" +currentDir);

		input = new FileInputStream("src/com/parser/Iso.properties");

		// load a properties file
		prop.load(input);
                String field1_name =prop.getProperty("field1_name");
                String field1_number =prop.getProperty("field1_number");
                String field1_length =prop.getProperty("field1_length");
                String field1_type =prop.getProperty("field1_type");
                
                StringBuilder result=new StringBuilder();
                String text =jTextArea1.getText();
                
                 String [] data =text.split(" ");
                
                String[] f_name=field1_name.split(",");
                String[] f_number=field1_number.split(",");
                String[] f_length=field1_length.split(",");
                String[] f_type=field1_type.split(",");
                
     String [] header = new String[18];
         	
for(int i=0;i<18;i++){
    header[i]=data[i];     
}     
            result.append("header "+Arrays.toString(header)+"\n");
            
            
               String [] msgid = new String[4];
 int j=0;             
for(int i=18;i<22;i++){
    msgid[j]=data[i]; 
    j++;    
}   

 result.append("msgid "+Arrays.toString(msgid)+"\n");

               String [] Map = new String[8];
               String [] B_Map = new String[8];
  j=0;             
for(int i=22;i<22+8;i++){
    Map[j]=String.format("%08d", Integer.parseInt(hexToBin(data[i])) ); 
    B_Map[j]=data[i]; 
    j++;    
}   
 result.append("B_Map "+Arrays.toString(Map)+"\n");
 result.append("Map "+Arrays.toString(B_Map)+"\n");
 
 
            System.out.println("B_Map "+Arrays.toString(Map)+"\n");
 System.out.println("Map "+Arrays.toString(B_Map)+"\n");
 
// get the property value and print it out
                
 //System.out.println(f_name.length+" "+f_number.length+" "+f_length.length+" "+f_type.length+" ");
 
 int loop_text=22+8;
 int field_num=1;

 // System.out.println("Map "+Arrays.toString(f_number));
  
  int last_filed=0;
 
for(int i=0;i<f_number.length;i++){
    
    
   
    field_num=Integer.parseInt(f_number[i]);
    
 if(field_num == 1)
         continue;
    
 if(field_num >= 64)
     break;

   last_filed=(field_num%8)!=0 ?(((int)field_num/8)*8):field_num-8;
   // System.out.println(field_num+"Last "+last_filed);

  
     if(Map[(field_num%8)!=0?((int)field_num/8):((int)field_num/8)-1].charAt(((field_num%8)!=0? field_num-(((int)field_num/8)*8)-1 : field_num-last_filed-1))=='1'){
         
       //  System.out.println("calcu "+field_num+" "+String.valueOf(((int)field_num/8)) +" "+String.valueOf((field_num%8)!=0? field_num-(((int)field_num/8)*8)-1 : 0));
    
   
        
        int length=0;
      
       StringBuilder value=new StringBuilder();
       j=0;
       
       if(f_type[i].charAt(1)=='0'){
             if(f_type[i].charAt(0)=='1' || f_type[i].charAt(0)=='2' || f_type[i].charAt(0)=='3' || f_type[i].charAt(0)=='4'){
 
           length=Integer.parseInt(f_length[i]);
        //   System.out.println(f_type[i].charAt(0)+""+f_type[i].charAt(1)+" length "+length);
       
       }else if(f_type[i].charAt(0)=='0' ){     
           length=Integer.parseInt(f_length[i]);
       //    System.out.println(f_type[i].charAt(0)+""+f_type[i].charAt(1)+" length "+length);  
       }
           
     }else if(f_type[i].charAt(1)=='1'){
             if(f_type[i].charAt(0)=='1' || f_type[i].charAt(0)=='2' || f_type[i].charAt(0)=='3' || f_type[i].charAt(0)=='4'){
 
           length=Integer.parseInt(f_length[i]);
         //  System.out.println(f_type[i].charAt(0)+""+f_type[i].charAt(1)+" length "+length);
       
       }else if(f_type[i].charAt(0)=='0' ){     
           length=Integer.parseInt(f_length[i]);
        //   System.out.println(f_type[i].charAt(0)+""+f_type[i].charAt(1)+" length "+length);  
       }else  if(f_type[i].charAt(0)=='5'){
 
           length=Integer.parseInt(data[loop_text]);
           loop_text=loop_text+1;
        //   System.out.println("53 length "+length);
       
       }else if(f_type[i].charAt(0)=='6'){
            
                   StringBuilder len=new StringBuilder();
                   len.append(data[loop_text]);
                   len.append(data[loop_text+1]);
 
           length=Integer.parseInt(len.toString());
           loop_text=loop_text+2;
        //   System.out.println("63 length "+length);
       
       }
           
     }else if(f_type[i].charAt(1)=='3'){
           
       if(f_type[i].charAt(0)=='5'){
 
           length=Integer.parseInt(data[loop_text]);
           loop_text=loop_text+1;
         //  System.out.println("53 length "+length);
       
       }else if(f_type[i].charAt(0)=='6'){
            
                   StringBuilder len=new StringBuilder();
                   len.append(data[loop_text]);
                   len.append(data[loop_text+1]);
 
           length=Integer.parseInt(len.toString());
           loop_text=loop_text+2;
        //   System.out.println("63 length "+length);
       
       }
       }
       
 
        for(int c=loop_text;c<loop_text+length;c++){
        //    System.out.println("com.parser.ISO_parer.jButton1MouseClicked( ) "+c);
         value.append(data[c]);
          
         } 
        loop_text=loop_text+length;
        
        byte[] bytes = DatatypeConverter.parseHexBinary(value.toString());
      String result_con= new String(bytes, "UTF-8");
        
          result.append(field_num+" {"+length+"} "+f_name[i]+" "+value.toString()+" {"+result_con+"} "+"\n");
    }
    
    
    
    
    
    

 
}


jTextArea2.setText(result.toString());
StringSelection selection = new StringSelection(result.toString());
Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
clipboard.setContents(selection, selection);       

try(PrintWriter out = new PrintWriter("D:\\GuadianPro\\Core\\S_MISR\\File\\parser\\texts.txt")  ){
    out.println(result.toString());
}
                
	} catch (IOException ex) {
		ex.printStackTrace();
	} finally {
		if (input != null) {
			try {
				input.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
    }//GEN-LAST:event_jButton1MouseClicked

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(ISO_parer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(ISO_parer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(ISO_parer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(ISO_parer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new ISO_parer().setVisible(true);
            }
        });
    }
    
    
    static String hexToBin(String s) {
  return new BigInteger(s, 16).toString(2);
}

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JTextArea jTextArea1;
    private javax.swing.JTextArea jTextArea2;
    // End of variables declaration//GEN-END:variables
}
