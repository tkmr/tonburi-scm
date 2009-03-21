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