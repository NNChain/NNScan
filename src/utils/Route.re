// Tab types kept for backward compatibility with page files (not used in routing)
type data_source_tab_t =
  | DataSourceExecute
  | DataSourceCode
  | DataSourceRequests
  | DataSourceRevisions;

type oracle_script_tab_t =
  | OracleScriptExecute
  | OracleScriptCode
  | OracleScriptBridgeCode
  | OracleScriptRequests;

type account_tab_t =
  | AccountDelegations
  | AccountUnbonding
  | AccountRedelegate;

type validator_tab_t =
  | ProposedBlocks
  | Delegators
  | Reports
  | Reporters;

type t =
  | NotFound
  | HomePage
  | TxHomePage
  | TxIndexPage(Hash.t)
  | BlockHomePage
  | BlockIndexPage(int)
  | AccountIndexPage(Address.t, account_tab_t)
  | ValidatorHomePage
  | ValidatorIndexPage(Address.t, validator_tab_t);

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path, url.hash) {
  | (["txs"], _) => TxHomePage
  | (["tx", txHash], _) => TxIndexPage(Hash.fromHex(txHash))
  | (["validators"], _) => ValidatorHomePage
  | (["blocks"], _) => BlockHomePage
  | (["block", blockHeight], _) =>
    let blockHeightIntOpt = blockHeight |> int_of_string_opt;
    switch (blockHeightIntOpt) {
    | Some(block) => BlockIndexPage(block)
    | None => NotFound
    };
  | (["account", address], hash) =>
    let urlHash = (
      fun
      | "unbonding" => AccountUnbonding
      | "redelegate" => AccountRedelegate
      | _ => AccountDelegations
    );
    switch (address |> Address.fromBech32Opt) {
    | Some(address) => AccountIndexPage(address, urlHash(hash))
    | None => NotFound
    };
  | (["validator", address], hash) =>
    let urlHash = (
      fun
      | "delegators" => Delegators
      | "reporters" => Reporters
      | "proposed-blocks" => ProposedBlocks
      | _ => Reports
    );
    switch (address |> Address.fromBech32Opt) {
    | Some(address) => ValidatorIndexPage(address, urlHash(hash))
    | None => NotFound
    };
  | ([], _) => HomePage
  | (_, _) => NotFound
  };

let toString =
  fun
  | TxHomePage => "/txs"
  | TxIndexPage(txHash) => {j|/tx/$txHash|j}
  | ValidatorHomePage => "/validators"
  | BlockHomePage => "/blocks"
  | BlockIndexPage(height) => {j|/block/$height|j}
  | AccountIndexPage(address, AccountDelegations) => {
      let addressBech32 = address |> Address.toBech32;
      {j|/account/$addressBech32#delegations|j};
    }
  | AccountIndexPage(address, AccountUnbonding) => {
      let addressBech32 = address |> Address.toBech32;
      {j|/account/$addressBech32#unbonding|j};
    }
  | AccountIndexPage(address, AccountRedelegate) => {
      let addressBech32 = address |> Address.toBech32;
      {j|/account/$addressBech32#redelegate|j};
    }
  | ValidatorIndexPage(validatorAddress, Delegators) => {
      let validatorAddressBech32 = validatorAddress |> Address.toOperatorBech32;
      {j|/validator/$validatorAddressBech32#delegators|j};
    }
  | ValidatorIndexPage(validatorAddress, Reports) => {
      let validatorAddressBech32 = validatorAddress |> Address.toOperatorBech32;
      {j|/validator/$validatorAddressBech32#reports|j};
    }
  | ValidatorIndexPage(validatorAddress, Reporters) => {
      let validatorAddressBech32 = validatorAddress |> Address.toOperatorBech32;
      {j|/validator/$validatorAddressBech32#reporters|j};
    }
  | ValidatorIndexPage(validatorAddress, ProposedBlocks) => {
      let validatorAddressBech32 = validatorAddress |> Address.toOperatorBech32;
      {j|/validator/$validatorAddressBech32#proposed-blocks|j};
    }
  | HomePage => "/"
  | NotFound => "/notfound";

let redirect = (route: t) => ReasonReactRouter.push(route |> toString);

let search = (str: string) => {
  let len = str |> String.length;
  let capStr = str |> String.capitalize_ascii;

  (
    switch (str |> int_of_string_opt) {
    | Some(blockID) => Some(BlockIndexPage(blockID))
    | None =>
      if (str |> Js.String.startsWith("nncvaloper")) {
        Some(ValidatorIndexPage(str |> Address.fromBech32, Reports));
      } else if (str |> Js.String.startsWith("nnc")) {
        Some(AccountIndexPage(str |> Address.fromBech32, AccountDelegations));
      } else if (len == 64 || str |> Js.String.startsWith("0x") && len == 66) {
        Some(TxIndexPage(str |> Hash.fromHex));
      } else if (capStr |> Js.String.startsWith("B")) {
        let%Opt blockID = str |> String.sub(_, 1, len - 1) |> int_of_string_opt;
        Some(BlockIndexPage(blockID));
      } else {
        None;
      }
    }
  )
  |> Belt_Option.getWithDefault(_, NotFound);
};
