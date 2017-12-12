#use "topfind";;
#require "lwt";;
#require "cohttp";;
#require "lwt";;
#require "cohttp.lwt";;
#require "lwt.unix";;

open Lwt
open Cohttp
open Cohttp_lwt_unix

let uri = ""
;;

let headers = Header.of_list [("Token", ""); ("Content-Type", "application/x-www-form-urlencoded")]
;;

let message branch = "@help " ^ branch
;;

let req b =
  let reqbody = Cohttp_lwt_body.of_string ("body=" ^ b) in
  Client.post ~body:reqbody ~headers:headers (Uri.of_string uri) >>= fun (resp, body) ->
  let code = resp |> Response.status |> Code.code_of_status in
  Printf.printf "Response code: %d\n" code;
  Printf.printf "Headers: %s\n" (resp |> Response.headers |> Header.to_string);
  body |> Cohttp_lwt_body.to_string >|= fun body ->
  Printf.printf "Body of length: %d\n" (String.length body);
  body

let () =
  let body = Lwt_main.run (req (message "develop")) in
  print_endline ("Received body\n" ^ body)
