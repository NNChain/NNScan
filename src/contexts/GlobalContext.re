type t = {financial: PriceHook.t};

let context = React.createContext(ContextHelper.default);

[@react.component]
let make = (~children) => {
  let client = React.useContext(ClientContext.context);
  
  // Initialize with default values so cards show immediately
  let defaultFinancial = {
    PriceHook.usdPrice: 1.0,
    PriceHook.usdMarketCap: 132_000_000.0,
    PriceHook.usd24HrChange: 0.0,
    PriceHook.btcPrice: 1.0 /. 50000.0, // Default BTC price
    PriceHook.btcMarketCap: (1.0 /. 50000.0) *. 132_000_000.0,
    PriceHook.circulatingSupply: 132_000_000.0,
  };
  
  let (financialOpt, setFinancialOpt) = React.useState(_ => Some(defaultFinancial));

  React.useEffect0(() => {
    let fetchData = () => {
      let _ =
        PriceHook.getBandInfo(client)
        |> Js.Promise.then_(bandInfoOpt => {
             switch (bandInfoOpt) {
             | Some(financial) => setFinancialOpt(_ => Some(financial))
             | None => () // Keep existing data if API fails
             };
             Promise.ret();
           });
      ();
    };

    // Fetch immediately on mount
    fetchData();
    // Update every 60 seconds
    let intervalID = Js.Global.setInterval(fetchData, 60000);
    Some(() => Js.Global.clearInterval(intervalID));
  });

  let data = {
    switch (financialOpt) {
    | Some(financial) => Some({financial: financial})
    | None => Some({financial: defaultFinancial}) // Fallback to default if None
    };
  };

  React.createElement(
    React.Context.provider(context),
    {
      "value": Sub.resolve(data->Belt_Option.getExn), // Always resolve (never Loading)
      "children": children,
    },
  );
};
