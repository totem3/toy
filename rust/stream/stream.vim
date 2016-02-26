function! Handler(chan, msg) abort
  for line in split(a:msg, "\n")
    let wn=bufwinnr("tmp")
    if wn == -1
      return 0
    endif
    exe wn 'wincmd w'
    1put!=a:msg
  endfor
  redraw
endfunction

if bufwinnr("tmp") == -1
  new tmp
  set buftype=nofile bufhidden=hide noswapfile
  wincmd p
endif

if exists('s:handle')
  call ch_close(s:handle)
endif
let s:handle = ch_open("127.0.0.1:8888", {"mode": "raw", "callback": "Handler"})
