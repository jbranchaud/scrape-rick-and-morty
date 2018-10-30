open Lwt;
open Cohttp;
open Cohttp_lwt_unix;
open Soup;

let print_titles = titles =>
  titles |> List.iter(title => print_endline("- " ++ title));

let main = () => {
  let fetch = {
    /* lib/Index.re */
    let url =
      Uri.of_string(
        "https://en.wikipedia.org/wiki/Rick_and_Morty_(season_1)",
      );
    Client.get(url)
    >>= (
      ((resp, body)) =>
        Cohttp_lwt.Body.to_string(body)
        >>= (
          str_body => {
            let soup = parse(str_body);
            let pageTitle = soup $ "title" |> R.leaf_text;

            let table = soup $ ".wikiepisodetable";
            let episodeTitleNodes = soup $$ "tr.vevent td.summary";
            let episodeTitles =
              episodeTitleNodes
              |> to_list
              |> List.map(node =>
                   node |> texts |> List.fold_left((a, b) => a ++ b, "")
                 );

            print_endline(pageTitle);
            print_titles(episodeTitles);

            return(body);
          }
        )
    );
  };

  Lwt_main.run(fetch) |> ignore;
};