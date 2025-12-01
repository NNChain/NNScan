let int64 = json => {
  // Handle both string and number formats from backend
  switch (Js.Json.decodeString(json)) {
  | Some(str) => int_of_string(str)
  | None =>
    switch (Js.Json.decodeNumber(json)) {
    | Some(num) => int_of_float(num)
    | None => raise(Failure("int64: expected string or number"))
    }
  };
};
let string = json => json |> Js.Json.decodeString |> Belt.Option.getExn;
let jsonToStringExn = jsonOpt =>
  jsonOpt |> Belt.Option.getExn |> Js.Json.decodeString |> Belt.Option.getExn;
let stringExn = (stringOpt: option(string)) => stringOpt |> Belt_Option.getExn;
let buffer = json =>
  json
  |> Js.Json.decodeString
  |> Belt.Option.getExn
  |> Js.String.substr(~from=2)
  |> JsBuffer.fromHex;

let timeS = json => {
  json
  |> Js.Json.decodeNumber
  |> Belt.Option.getExn
  |> int_of_float
  |> MomentRe.momentWithUnix
  |> MomentRe.Moment.defaultUtc;
};

let fromUnixSecondOpt = timeOpt => {
  timeOpt->Belt_Option.map(x => {x |> MomentRe.momentWithUnix |> MomentRe.Moment.defaultUtc});
};

let timeMS = json => {
  json
  |> Js.Json.decodeNumber
  |> Belt.Option.getExn
  |> MomentRe.momentWithTimestampMS
  |> MomentRe.Moment.defaultUtc;
};

let timestamp = json => {
  json |> Js.Json.decodeString |> Belt.Option.getExn |> MomentRe.momentUtcDefaultFormat;
};

let timeNS = json => {
  json
  |> Js.Json.decodeNumber
  |> Belt.Option.getExn
  |> (nanoSec => nanoSec /. 1e6)
  |> MomentRe.momentWithTimestampMS
  |> MomentRe.Moment.defaultUtc;
};

let timestampOpt = Belt_Option.map(_, timestamp);

let timestampWithDefault = jsonOpt =>
  jsonOpt
  |> Belt_Option.flatMap(_, x => Some(timestamp(x)))
  |> Belt.Option.getWithDefault(_, MomentRe.momentNow());

let optionBuffer = Belt_Option.map(_, buffer);

let optionTimeMS = Belt_Option.map(_, timeMS);

let optionTimeS = Belt_Option.map(_, timeS);

let optionTimeSExn = timeSOpt => timeSOpt |> Belt_Option.getExn |> timeS;

let bool = json => json |> Js.Json.decodeBoolean |> Belt.Option.getExn;

let hash = json =>
  json |> Js.Json.decodeString |> Belt.Option.getExn |> Js.String.substr(~from=2) |> Hash.fromHex;

let coinRegEx = "([0-9]+)([a-z][a-z0-9/]{2,31})" |> Js.Re.fromString;

let intToCoin = int_ => int_ |> float_of_int |> Coin.newUBANDFromAmount;

let coin = json => {
  // Handle both string and number formats from backend
  switch (Js.Json.decodeString(json)) {
  | Some(str) => str |> float_of_string |> Coin.newUBANDFromAmount
  | None =>
    switch (Js.Json.decodeNumber(json)) {
    | Some(num) => num |> Coin.newUBANDFromAmount
    | None => raise(Failure("coin: expected string or number"))
    }
  };
};

let coinExn = jsonOpt => {
  jsonOpt
  |> Belt_Option.flatMap(_, Js.Json.decodeString)
  |> Belt.Option.getExn
  |> float_of_string
  |> Coin.newUBANDFromAmount;
};

let floatExn = jsonOpt => {
  jsonOpt |> Belt_Option.flatMap(_, Js.Json.decodeString) |> ((opt) => switch (opt) {
    | Some(value) => value
    | None => "0"
  }) |> float_of_string
};

let floatOpt = jsonOpt => {
  jsonOpt |> Belt_Option.flatMap(_, Js.Json.decodeString) |> Belt.Option.map(_, float_of_string)
};

let coinWithDefault = jsonOpt => {
  // Handle both string and number formats, with default to 0
  switch (jsonOpt) {
  | None => Coin.newUBANDFromAmount(0.)
  | Some(json) =>
    switch (Js.Json.decodeString(json)) {
    | Some(str) => str |> float_of_string |> Coin.newUBANDFromAmount
    | None =>
      switch (Js.Json.decodeNumber(json)) {
      | Some(num) => num |> Coin.newUBANDFromAmount
      | None => Coin.newUBANDFromAmount(0.)
      }
    }
  };
};

let float_of_string_opt = (str: string) => {
  try (Some(float_of_string(str))) {
  | _ => None
  };
};

let coins = str =>
  // Handle null/empty strings gracefully
  if (str == "" || str == "null" || str == "undefined") {
    [];
  } else {
    str
    |> Js.String.trim
    |> Js.String.toLowerCase
    |> Js.String.split(",")
    |> Belt_List.fromArray
    |> Belt_List.keepMap(_, coin => {
         let trimmed = coin |> Js.String.trim;
         if (trimmed == "") {
           None;
         } else {
           switch (trimmed |> Js.Re.exec_(coinRegEx)) {
           | Some(result) =>
             let captures = Js.Re.captures(result);
             switch (
               captures[1] |> Js.Nullable.toOption,
               captures[2] |> Js.Nullable.toOption,
             ) {
             | (Some(amountStr), Some(denom)) =>
               switch (amountStr |> float_of_string_opt) {
               | Some(amount) => Some({Coin.denom, amount})
               | None => None
               }
             | _ => None
             }
           | None => None
           }
         }
       })
  };

let addressExn = jsonOpt => jsonOpt |> Belt_Option.getExn |> Address.fromBech32;
let addressOpt = jsonOpt => jsonOpt |> Belt.Option.map(_, Address.fromBech32);

let numberWithDefault = jsonOpt =>
  jsonOpt |> Belt_Option.flatMap(_, Js.Json.decodeNumber) |> Belt.Option.getWithDefault(_, 0.0);

let floatWithDefault = jsonOpt =>
  jsonOpt
  |> Belt_Option.flatMap(_, Js.Json.decodeString)
  |> Belt.Option.mapWithDefault(_, 0., float_of_string);

let floatString = json => {
  // Handle both string and number formats from backend
  switch (Js.Json.decodeString(json)) {
  | Some(str) => float_of_string(str)
  | None =>
    switch (Js.Json.decodeNumber(json)) {
    | Some(num) => num
    | None => raise(Failure("floatString: expected string or number"))
    }
  };
};

