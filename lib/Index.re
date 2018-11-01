open Lwt;
open Cohttp;
open Cohttp_lwt_unix;
open Soup;

let print_titles = titles =>
  titles |> List.iter(title => print_endline("- " ++ title));

let print_title_and_description = (title, description) => {
  print_endline("- " ++ title);
  print_endline("  " ++ description ++ "\n");
};

let write_descriptions_to_file = (descriptions: list(string)) => {
  let file =
    Unix.openfile(
      "./episode-descriptions.txt",
      [Unix.O_CREAT, Unix.O_WRONLY],
      0o666,
    );
  descriptions
  |> List.iter(description => {
       let desc_bytes = description |> Bytes.of_string;
       Unix.write(file, desc_bytes, 0, Bytes.length(desc_bytes));
       ignore();
     });
  Unix.close(file);
};

let main = () => {
  let fetch = {
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
            let episodeTitleNodes = table $$ "tr.vevent td.summary";
            let episodeTitles =
              episodeTitleNodes
              |> to_list
              |> List.map(node =>
                   node |> texts |> List.fold_left((a, b) => a ++ b, "")
                 );

            let episodeDescriptionNodes = table $$ "tr td.description";
            let episodeDescriptions =
              episodeDescriptionNodes
              |> to_list
              |> List.map(node =>
                   node |> texts |> List.fold_left((a, b) => a ++ b, "")
                 );

            print_endline(pageTitle);
            List.iter2(
              print_title_and_description,
              episodeTitles,
              episodeDescriptions,
            );
            /* print_titles(episodeTitles); */

            write_descriptions_to_file(episodeDescriptions);

            return(body);
          }
        )
    );
  };

  Lwt_main.run(fetch) |> ignore;
};