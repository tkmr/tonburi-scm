(define (number? x)
  (equalType? x 1))

(define (and a b)
  (if a
      b
      #f))

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
    ((_ ...)
     ((lambda ()
        (lambda ()
          ...))))))

(define (force delayed)
  (delayed))

(define delayprint (delay (print "this is delayed print")))
(print "---delayed---")

(force delayprint)

