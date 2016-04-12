#lang racket

((lambda (n)
    ((lambda (fact)
      (fact fact n))
      (lambda (ft k)
        (if (= k 1)
          1
          (* k (ft ft (- k 1)))))))
 5)


(define (_fib x)
  (cond ((= x 0) 0)
        ((= x 1) 1)
        (else (+ (fib (- x 1)) (fib (- x 2))))))

(define fib (lambda (n)
              ((lambda (fib)
                (fib fib n))
                (lambda (ft k)
                  (cond ((= k 0) 0)
                        ((= k 1) 1)
                        (else (+ (fib (- k 1)) (fib (- k 2)))))))))

(fib 1)
(fib 2)
(fib 3)
(fib 4)
(fib 5)
(fib 6)
(fib 7)
(fib 8)
(fib 9)
(fib 10)
