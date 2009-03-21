(define (number? x)
  (equalType? x 1))

(define (or a b)
  (if a #t (if b #t #f)))

(define (list? x)
  (equalType? x (list 1 2)))

(define (map func lists)
  (if (null? lists)
      nil
      (cons (func (car lists))
            (map func (cdr lists)))))

(define (pow n)
  (* n n))

(define (list ar . dr)
  (cons ar dr))

(define (sum i . remain)
  (if (null? remain)
      i
      (+ i (sum (car remain)
                (cdr remain)))))

(print (list? (list 1 2 3)))
(print (list? 1))
(print (atom? (list 1 2 3)))
(print (atom? 1))
(print (null? (list 1 2 3)))
(print (null? 1))
(print (null? nil))
(print (null? (cdr (cdr (cons 1000 1000)))))
(print (sum 1 2 3 4 5 6))
(print (map pow (list 1 2 3 4 5)))

(define (generate-list start end i)
  (define (iter start)
    (if (< end start)
        end
        (cons start
              (iter (+ start i)))))
  (iter start))

(print (map
        (lambda (n) (* n 2))
        (generate-list 1 10 2)))

(define (collect func list)
  (if (null? (cdr list))
      (car list)
      (func (car list)
            (collect func (cdr list)))))

(print (collect
        (lambda (a b) (+ a b))
        (generate-list 1 20 1)))

(define-syntax nil!
  (syntax-rules (x)
    ((_ x) (set! x 112233))))

(define a 1)
(set! a 99)
(print a)
(nil! a)
(print a)

(define-syntax delay
  (syntax-rules (x)
    ((_ b ...)
     ((lambda ()
        (lambda ()
          b ...))))))

(define (force delayed)
  (delayed))

(print "---before delayed---")
(define delayprint (delay (print "this is delayed print")))
(print "---delayed---")

(force delayprint)

(define-syntax incf
  (syntax-rules (x)
    ((_ x) (set! x (+ x 1)))
    ((_ x i) (set! x (+ x i)))))

(define x1 100)
(print x1)
(incf x1)
(print x1)
(incf x1 90)
(print x1)

(define-syntax mc_test
  (syntax-rules (x)
    ((_ a ...) (list a ...))))

(print (mc_test 1 2 3 4 5 6 7))

(define-syntax and
  (syntax-rules (x)
    ((and) #t)
    ((and test) test)
    ((and test1 test2)
     (if test1
         test2
         #f))
    ((and test1 test2 ...)
     (if test1
         (and test2 ...)
         #f))))

(print (and #t #t #t #t))

(define-syntax let
  (syntax-rules ()
    ((let ((_k _v) ...) _b ...)
     ((lambda (_k ...) _b ...) _v ...))))

(let ((aa "world"))
  (print "hello")
  (print "hoge"))

(define-syntax letrec
  (syntax-rules ()
    ((letrec ((var1 init1)) body ...)
     ((lambda (var1)
        (let ((temp1 "undefined"))
          (set! temp1 var1)
          body ...)) init1))
    ((letrec ((var1 init1) params ...) body ...)
     (letrec (params ...)
       ((lambda (var1)
          (let ((temp1 "undefined"))
            (set! temp1 var1)
            body ...)) init1)))))

(letrec ((lc1 "hello") (lc2 lc1))
  (print lc1)
  (print lc2))

((lambda (x y)
   (print x)
   (print y)) 12 34)
