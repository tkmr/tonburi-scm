(print "start: lazy eval-----")
(define unless
  (lambda (p t f)
    (if p f t)))

(print "1: lazy eval-----")
(define hoge
  (unless (= 1 0)
          (print "OK")
          (print "NG")))

(print "2: lazy eval-----")
(print hoge)
(print "3: lazy eval-----")

(define count 1)
(define (id x)
  (set! count (+ count 1))
  x)

(print count)
(define w (id (id 10)))
(print count)
(print w)
(print count)

(print "-----------")
(define (add i)
  (print i)
  (+ i i))

(add (add 10))

(print "stream:-----------")

(define (touch x)
  (car (cons x nil)))
(define (square x)
  (* x x))
(define (divisible? x n)
  (< (- x (* (/ x n) n)) 1))

((lambda ()
   (define (cons a b)
     (lambda (x) (x a b)))

   (define (car pair)
     (pair (lambda (a b) (touch a))))

   (define (cdr pair)
     (pair (lambda (a b) (touch b))))

   (define (list-ref items n)
     (print (car items))
     (if (< n 1)
         (car items)
         (list-ref (cdr items) (- n 1))))

   (define (list-filter p items)
     (if (p (car items))
         (cons (car items)
               (list-filter p (cdr items)))
         (list-filter p (cdr items))))

   (define (list-map proc items)
     (cons (proc (car items))
           (list-map proc (cdr items))))

   (define (lists-map proc list1 list2)
     (cons (proc (car list1) (car list2))
           (lists-map proc (cdr list1) (cdr list2))))

   (define (scale-lists items factor)
     (list-map (lambda (x) (* x factor)) items))

   (define (add-lists list1 list2)
     (lists-map + list1 list2))

   (define ones
     (cons 1 ones))
   (define numbers
     (cons 1 (add-lists ones numbers)))
   (define even-numbers
     (list-filter (lambda (x) (< (- x (* (/ x 2) 2)) 1)) numbers))

   (list-ref even-numbers 10)

   (define fib-numbers
     (cons 1
           (cons 1
                 (add-lists (cdr fib-numbers)
                            fib-numbers))))

   (list-ref fib-numbers 10)

   (define primes
     (list-filter prime?
                  (list-filter (lambda (n)(> n 1)) numbers)))

   (define (prime? n)
     (define (iter ps)
       (if (> (square (car ps)) n)
           #t
           (if (divisible? n (car ps))
               #f
               (iter (cdr ps)))))
     (iter primes))

   (list-ref primes 10)

   "done"
 ))
