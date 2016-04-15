#lang racket/gui

(define frame (new frame% [label "Example"]
                          [width 640]
                          [height 400]))
(define msg (new message% [parent frame]
                          [label "No events so far..."]))

(send frame show #t)
