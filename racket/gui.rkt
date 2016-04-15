#lang racket/gui

(define frame (new frame% [label "Example"]
                          [width 640]
                          [height 400]))
(define font (make-font #:size 20.0 #:weight 'bold #:underlined? #t))
(define msg (new message% [parent frame]
                          [font font]
                          [label "No events so far..."]))

(send frame show #t)
