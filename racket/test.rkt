#lang racket

(require rackunit)
(require rackunit/gui)
(require rackunit/text-ui)

(test/gui (test-suite
   "suite example"
   (test-case
     "test example"
     (check-eq? 1 1))
   (test-case
     "another test"
     (check-not-eq? 4 3)))
  #:wait? #t
  )
 
(run-tests (test-suite
  "another suite"
  (test-case
    "another suite test"
    (check-false #f))))

