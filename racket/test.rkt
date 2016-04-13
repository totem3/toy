#lang racket

(require rackunit)
(require rackunit/gui)
(require rackunit/text-ui)

(define debug-mode (make-parameter #f))
(define filters (make-parameter null))
(define run
  (command-line
    #:program "test"
    #:once-each
    [("-d" "--debug") "Enable debug output"
                       (debug-mode #t)]
    #:multi
    [("-f" "--filter") f "Run only matching test"
                       (filters (cons f (filters)))]))

(define suites (list
(test-suite
   "suite example"
   (test-case
     "test example"
     (check-eq? 1 1))
   (test-case
     "another test"
     (check-not-eq? 4 3)))
(test-suite
  "another suite"
  (test-case
    "another suite test"
    (check-false #f)))
))


(if (null? (filters))
  (for-each run-tests suites)
  (for ([suite suites])
    (when (member #t (map (lambda (x) (string-contains? (rackunit-test-suite-name suite) x)) (filters)))
      (run-tests suite))
    )
  )
