(* ENS, Système et réseau, TP 8
   Louis Mandel & Timothy Bourke
*)

(* Configuration *)

type mode = Server | Client

let mode, server_name, port,
    length, width,
    pad_size, pad_size_2, pad_speed,
    vx_init, vy_init, v_max  =
  let _ = Random.self_init () in
  let length = ref 800 in
  let width = ref 600 in
  let pad_size = ref 100 in
  let k_pad = ref 1. in
  let vx, vy = let a = Random.float 3.14 in ref (cos a), ref (sin a) in
  let k_ball = ref 2. in
  let mode = ref Server in
  let server_name = ref "localhost" in
  let port = ref 12345 in
  let spec =
    [ "-client", Arg.Unit (fun () -> mode := Client), "run as a client" ;
      "-server-name", Arg.Set_string server_name, "set the server name" ;
      "-port", Arg.Set_int port, "n (default 12345)";
      "-length", Arg.Set_int length, "set the length of the game area";
      "-width", Arg.Set_int width, "set the width of the game area";
      "-pad", Arg.Set_int pad_size, "set the size of the pad";
      "-pspeed", Arg.Set_float k_pad, "speed factor for the pad";
      "-alpha", Arg.Float (fun a -> vx := cos a; vy := sin a),
         "set the initial angle";
      "-speed", Arg.Set_float k_ball, "speed factor for the ball"; ]
  in
  let usage =
    ("Usage: "^Sys.argv.(0)^" [options] \n"^
     "Options:")
  in
  Arg.parse spec (fun _ -> Arg.usage spec usage; exit 1) usage;
  vx := !k_ball *. !vx;
  vy := !k_ball *. !vy;
  (!mode, !server_name, !port,
   float_of_int !length, float_of_int !width,
   !pad_size, float_of_int !pad_size /. 2., 4. *. !k_pad,
   !vx, !vy, !k_ball)

(* --------------------------------------------------------- *)

type player = P1 | P2

type direction = Left | Right

type state =
    { pad1: float;
      pad2: float;
      ball_p: float * float;
      ball_v: float * float; }

exception Win of player

(* --------------------------------------------------------- *)

let move_pad player pre_y dir =
  match player, dir with
  | (P1, Some Left)
  | (P2, Some Right) -> pre_y +. pad_speed
  | (P1, Some Right)
  | (P2, Some Left) -> pre_y -. pad_speed
  | _, None -> pre_y

let move_ball (pre_vx, pre_vy) (pre_x, pre_y) pad1 pad2 =
  let pong pad = pad -. pad_size_2 <= pre_y && pre_y <= pad +. pad_size_2 in
  let vx =
    if pre_x <= 0. && pong pad1 || pre_x >= length && pong pad2 then -. pre_vx
    else pre_vx
  in
  let vy =
    if pre_y <= 0. || pre_y >= width then -. pre_vy
    else pre_vy
  in
  let x, y = (pre_x +. vx,  pre_y +. vy) in
  (vx, vy), (x, y)

let step pre_state (act1, act2) =
  let pad1 = move_pad P1 pre_state.pad1 act1 in
  let pad2 = move_pad P2 pre_state.pad2 act2 in
  let v, p =
    move_ball pre_state.ball_v pre_state.ball_p pre_state.pad1 pre_state.pad2
  in
  { pad1 = pad1; pad2 = pad2; ball_v = v; ball_p = p; }

let play read_pads draw init_state =
  let rec play pre_state =
    let actions = read_pads () in
    let state = step pre_state actions in
    draw state;
    if fst state.ball_p < -. v_max then raise (Win P2);
    if fst state.ball_p > length +. v_max then raise (Win P1);
    play state
  in
  play init_state


(* --------------------------------------------------------- *)

module type INTERFACE = sig
  type win
  val init_graph : unit -> win
  val draw_pad : win -> player -> float -> unit
  val draw_ball : win -> float * float -> unit
  val clear_graph : win -> unit
  val synchronize : win -> unit
  val poll_key : win -> char option
  val sleep : float -> unit
  val close : win -> unit
end

module OCamlGraphics : INTERFACE = struct

  type win = unit

  let init_graph () =
    Graphics.open_graph
      (Printf.sprintf " %dx%d" (int_of_float length) (int_of_float width));
    Graphics.auto_synchronize false

  let draw_pad () player pos =
    let pad_width = 4 in
    let x =
      match player with
      | P1 -> 0
      | P2 -> int_of_float length - pad_width
    in
    let y = int_of_float (pos -. pad_size_2) in
    Graphics.fill_rect x y pad_width pad_size

  let draw_ball () (x, y) =
    let x, y = int_of_float x, int_of_float y in
    Graphics.fill_circle x y 5

  let clear_graph = Graphics.clear_graph

  let synchronize = Graphics.synchronize

  let poll_key () =
    if Graphics.key_pressed () then Some (Graphics.read_key ())
    else None

  let sleep n =
    try ignore (Unix.select [Unix.stdin] [] [] n)
    with Unix.Unix_error(Unix.EINTR,_,_) -> ()
       | Unix.Unix_error(Unix.EAGAIN,_,_) -> ()

  let close _ = ()
end

(*
   To use the SDL interface:
   1. Install OPAM:
      sh <(curl -sL https://raw.githubusercontent.com/ocaml/opam/master/shell/install.sh)
   2. Install a local "switch" (takes a few minutes):
      opam switch create 4.10.0
      eval $(opam env)            # VERY IMPORTANT
   3. Install basic packages
      opam install ocamlbuild ocamlfind merlin
   4. Install the ocamlsdl package
      opam install ocamlsdl
   5. Uncomment the module below
   6. Replace the line
        module MyGraphics = MakeGraphics (OCamlGraphics)
      with
        module MyGraphics = MakeGraphics (SdlGraphics)
   7. Uncomment the ",sdl" package in Makefile
 *)
(*
module SdlGraphics : INTERFACE = struct
  type win = Sdlvideo.surface

  let draw_pad screen player pos =
    let pad_width = 4 in
    let x =
      match player with
      | P1 -> 0
      | P2 -> int_of_float length - pad_width
    in
    let y = int_of_float (pos -. pad_size_2) in
    let color = Sdlvideo.(map_RGB screen black) in
    Sdlvideo.(fill_rect
      ~rect:{ r_x = x;
              r_y = y;
              r_w = pad_width;
              r_h = pad_size} screen color)

  let draw_ball screen (x, y) =
    let color = Sdlvideo.(map_RGB screen black) in
    Sdlvideo.(fill_rect
      ~rect:{ r_x = int_of_float x;
              r_y = int_of_float y;
              r_w = 4;
              r_h = 4} screen color)

  let clear_graph screen =
    let info = Sdlvideo.surface_info screen in
    let color = Sdlvideo.(map_RGB screen white) in
    Sdlvideo.(fill_rect
      ~rect:{ r_x = 0;
              r_y = 0;
              r_w = info.w;
              r_h = info.h} screen color)

  let init_graph () =
    Sdl.init [`VIDEO];
    let screen =
      Sdlvideo.set_video_mode
        ~w:(int_of_float length) ~h:(int_of_float width) [`DOUBLEBUF]
    in
    Sdlkey.enable_unicode true;
    Sdlkey.enable_key_repeat ();
    clear_graph screen;
    screen

  let synchronize = Sdlvideo.flip

  let poll_key _ =
    let open Sdlevent in
    if has_event () then
      match wait_event () with
      | KEYDOWN {keysym=key} -> Some (Sdlkey.char_of_key key)
      | _ -> None
    else None

  let sleep n =
    Sdltimer.delay (int_of_float (n *. 1000.0))

  let close _ = Sdl.quit ()
end

*)
(*
   To use the Curses interface:
   1. Install OPAM (if necessary):
      sh <(curl -sL https://raw.githubusercontent.com/ocaml/opam/master/shell/install.sh)
   2. Install a local "switch" (if necessary; takes a few minutes):
      opam switch create 4.10.0
      eval $(opam env)            # VERY IMPORTANT
   3. Install basic packages
      opam install ocamlbuild ocamlfind merlin
   4. Install the curses package
      opam install curses
   5. Uncomment the module below
   6. Replace the line
        module MyGraphics = MakeGraphics (OCamlGraphics)
      with
        module MyGraphics = MakeGraphics (CursesGraphics)
   7. Uncomment the ",curses" package in Makefile
 *)
(*
module CursesGraphics : INTERFACE = struct
  type win = {
    window : Curses.window;
    acs    : Curses.Acs.acs;
  }

  let term_width = 80
  let term_height = 30

  let scale_x = (float_of_int term_width) /. length
  let scale_y = (float_of_int term_height) /. width

  let pad_length_2 = int_of_float (pad_size_2 *. scale_y)

  let paddle_color = 1
  let ball_color = 2
  let border_color = 3

  let draw_court { acs } =
    let ul = int_of_char '+' in
    let ur, bl, br = ul, ul, ul in
    let hline, vline = int_of_char '-', int_of_char '|' in
    let border_attr = Curses.A.color_pair border_color in
    ignore Curses.(attr_on border_attr);
    ignore Curses.(mvaddch 0 0 ul);
    ignore Curses.(mvaddch 0 term_width ur);
    ignore Curses.(mvaddch term_height 0 bl);
    ignore Curses.(mvaddch term_height term_width br);
    for x = 1 to term_width - 1 do
      ignore (Curses.mvaddch 0 x hline);
      ignore (Curses.mvaddch term_height x hline)
    done;
    for y = 1 to term_height - 1 do
      ignore (Curses.mvaddch y 0 vline);
      ignore (Curses.mvaddch y term_width vline)
    done;
    ignore Curses.(attr_off border_attr)

  let draw_pad { acs } player pos =
    let block = int_of_char '=' in
    let x =
      match player with
      | P1 -> 0
      | P2 -> term_width
    in
    let y = int_of_float (pos *. scale_y) in
    let paddle_attr = Curses.A.color_pair paddle_color in
    ignore Curses.(attr_on paddle_attr);
    for v = y - pad_length_2 to y + pad_length_2 do
      ignore (Curses.mvaddch v x block)
    done;
    ignore Curses.(attr_off paddle_attr)

  let draw_ball { acs } (x, y) =
    let bullet = int_of_char '*' in
    let x, y = int_of_float (x *. scale_x), int_of_float (y *. scale_y) in
    let ball_attr = Curses.A.color_pair ball_color in
    ignore (Curses.attr_on ball_attr);
    ignore (Curses.mvaddch y x bullet);
    ignore (Curses.attr_off ball_attr)

  let clear_graph screen =
    Curses.clear ();
    draw_court screen

  let init_graph () =
    let w = {
      window = Curses.initscr ();
      acs = Curses.get_acs_codes ();
    } in
    ignore (Curses.start_color ());        (* enable colour *)
    ignore (Curses.cbreak ());             (* accept characters one-by-one *)
    ignore (Curses.noecho ());             (* do not echo typed characters *)
    ignore (Curses.curs_set 0);            (* do not show the cursor *)
    ignore (Curses.nodelay w.window true); (* nonblocking getch *)
    ignore (Curses.init_pair paddle_color Curses.Color.green Curses.Color.blue);
    ignore (Curses.init_pair ball_color   Curses.Color.red   Curses.Color.black);
    ignore (Curses.init_pair border_color Curses.Color.black Curses.Color.white);
    clear_graph w;
    w

  let synchronize _ =
    ignore (Curses.refresh ())

  let poll_key _ =
    let c = Curses.getch () in
    if c = -1 then None else Some (char_of_int c)

  let sleep n =
    try ignore (Unix.select [Unix.stdin] [] [] n)
    with Unix.Unix_error(Unix.EINTR,_,_) -> ()
       | Unix.Unix_error(Unix.EAGAIN,_,_) -> ()

  let close _ =
    Curses.endwin ()

end
*)

module MakeGraphics (Iface : INTERFACE) = struct
  include Iface

  let draw win state =
    clear_graph win;
    draw_ball win state.ball_p;
    draw_pad win P1 state.pad1;
    draw_pad win P2 state.pad2;
    synchronize win 

end

module MyGraphics = MakeGraphics (OCamlGraphics)

let draw_server fds win state =
  let s = Marshal.to_bytes (state: state) [] in
  let n = Bytes.length s in
  List.iter (fun fd -> ignore (Unix.write fd s 0 n)) fds;
  MyGraphics.draw win state

(* --------------------------------------------------------- *)


let actions_of_char c =
  match c with
  | 's' -> Some Left, None
  | 'f' -> Some Right, None
  | 'j' -> None, Some Left
  | 'l' -> None, Some Right
  | 'q' -> exit 0
  | _ -> None, None

let graphics_read_pads win =
  match MyGraphics.poll_key win with
  | None -> None, None
  | Some c ->
      actions_of_char c

(* --------------------------------------------------------- *)

module Server = struct
  let fd_read_pad fd =
    let buff = Bytes.make 1 'X' in
    ignore (Unix.read fd buff 0 1);
    actions_of_char (Bytes.get buff 0)

  let merge_action act act' =
    match act, act' with
    | None, None -> None
    | Some v, None | None, Some v -> Some v
    | Some v, Some v' -> Some v

  let merge_actions (act1, act2) (act1', act2') =
    merge_action act1 act1', merge_action act2 act2'

  let read_pads_sampling n fd1 fd2 () =
    let start = Unix.gettimeofday () in
    let rec read_pads_sampling t acc =
      try
        match Unix.select [ fd1; fd2; ] [] [] t with
        | [], [], [] -> acc
        | fds, [], [] ->
            let acts =
              List.fold_left
                (fun acts fd ->
                  let act1, act2 = fd_read_pad fd in
                  if fd = fd1 then merge_actions (act1, None) acts
                  else if fd = fd2 then merge_actions (None, act2) acts
                  else assert false)
                acc fds
            in
            continue acts
        | _ -> assert false
      with Unix.Unix_error ((Unix.EINTR | Unix.EAGAIN), _, _) -> continue acc
      | Unix.Unix_error (err, f, _) ->
          Format.eprintf "%s: %s@." f (Unix.error_message err);
          exit 1
    and continue acc =
      let now = Unix.gettimeofday () in
      let remaining = start +. n -. now in
      if remaining > 0.0 then read_pads_sampling remaining acc
      else acc
    in
    read_pads_sampling n (None, None)

  let server_init port =
    Format.printf "Ping server %s:%i@\n@?" (Unix.gethostname ()) port;
    let ip_addr = Unix.inet_addr_any in
    let addr = Unix.ADDR_INET (ip_addr, port) in
    let s = Unix.socket Unix.PF_INET Unix.SOCK_STREAM 0 in
    let () = Unix.setsockopt s Unix.SO_REUSEADDR true in
    let () = Unix.bind s addr in
    let () = Unix.listen s 2 in
    let fd1, _ = Unix.accept s in
    let fd2, _ = Unix.accept s in
    (fd1, fd2)


  let main () =
    let init =
      { pad1 = width /. 2.;
        pad2 = width /. 2.;
        ball_p = (length /. 2., width /. 2.);
        ball_v = (vx_init, vy_init); }
    in
    let fd1, fd2 = server_init port in
    let win = MyGraphics.init_graph () in
    at_exit (fun () -> MyGraphics.close win);
    try
      play
        (read_pads_sampling 0.01 fd1 fd2)
        (draw_server [fd1; fd2] win)
        init
    with Win P1 -> Format.printf "Player 1 wins!@."
    | Win P2 -> Format.printf "Player 2 wins!@."

end

module Client = struct

  let connect () =
    let host = Unix.gethostbyname server_name in
    let addr = Unix.ADDR_INET (host.Unix.h_addr_list.(0), port) in
    let s = Unix.socket Unix.PF_INET Unix.SOCK_STREAM 0 in
    let () = Unix.connect s addr in
    s

  let play fd win =
    let buff = Bytes.make 1 'X' in
    while true do
      match MyGraphics.poll_key win with
      | None -> MyGraphics.sleep 0.01
      | Some c -> begin
          Bytes.set buff 0 c;
          ignore (Unix.write fd buff 0 1)
        end
    done

  let display win ch =
    while true do
      let (state: state) = Marshal.from_channel ch in
      MyGraphics.draw win state
    done

  let main () =
    let fd = connect () in
    let win = MyGraphics.init_graph () in
    at_exit (fun () -> MyGraphics.close win);
    ignore (Thread.create (display win) (Unix.in_channel_of_descr fd));
    play fd win

end


(* --------------------------------------------------------- *)

let main =
  match mode with
  | Server -> Server.main ()
  | Client -> Client.main ()

