What this program does?
  This program compile programs written in the language Minima (created by the teacher just for the work). 
  
  This language's syntax can be formally defined by:
    
    func	::	cmd '\n' | cmd '\n' func
    
    cmd	::	att | if | ret
    
    att	::	var ':=' varc op varc
    
    if	::	'ifeq' varc varc num
    
    ret	::	'ret' varc
    
    var	::	'v' digito | 'p' digito
    
    varc	::	var | '$' snum
    
    op	::	'+' | '-' | '*'
    
    num	::	digito | num digito
    
    snum	::	[-] num
    
    digito	::	0' | '1' | '2' | '3' | '4' | '5' | '6' | '7'| '8' | '9'
  
  And has 3 instructions:
    Assigning to a variable a value
      var := varc1 op varc2
      
    An 'if'
      ifeq varc1 varc2 num
    
    And a return
      ret  varc
      
  This compiler does not treat problems with wrong syntax, he considers that every program written in Minima is syntatically correct
