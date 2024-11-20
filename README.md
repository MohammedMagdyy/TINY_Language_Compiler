"# TINY_Language_Compiler" 
##Scanner Part:

 #Inputs : 
  TINY language snippet code (multiple lines of code written in TINY language syntax)
  
 #Output: 
  List of ( tokenvalue, tokentype) in a file



##Parser Part:
Project Description: 
• Given the TINY grammar rules you should implement the TINY parser using recursive 
descent method.
 • You will need to convert grammar into EBNF form.
 • The output will be a complete syntax tree of the input source program

 
# Inputs : 
• List of ( tokenvalue, 
tokentype) Example:
 x ,IDENTIFIER
 :=, ASSIGN
 4,NUMBER
 • The input list should follow the same syntax as mentioned in the previous 
example tokenvalue , tokentype
 • Input list can be input through GUI textbox or by loading a text file
 
#Output: 
 1. State whether the statements are accepted by TINY language or not
 2. Draw Syntax tree on a GUI based application
 3. IF you do not support GUI ( and will lose GUI marks) you can output recognized 
structures by the TINY language parser into a file or on the console screen ( like 
drawing the syntax tree by describing it using statement names)
 
