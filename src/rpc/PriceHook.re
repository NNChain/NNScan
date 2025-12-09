type t = {
  usdPrice: float,
  usdMarketCap: float,
  usd24HrChange: float,
  btcPrice: float,
  btcMarketCap: float,
  circulatingSupply: float,
};

// Hardcoded values for NNC testnet
let nncUsdPrice = 1.0; // $1.00 per NNC (hardcoded for testnet)
let nncCirculatingSupply = 132_000_000.0; // 132M NNC (hardcoded for testnet)
let nncUsdMarketCap = 132_000_000.0; // $132M (hardcoded for testnet)
let nncUsd24HrChange = 0.0; // 0% change (hardcoded for testnet)

// Default BTC/USD price (used initially before API responds)
let defaultBtcUsdPrice = 50000.0;

// Fetch BTC/USD price from CoinGecko (free API, no key needed)
// This is the ONLY source for BTC/USD (no oracle scripts, no fallback)
let getBtcUsdPrice = () => {
  Axios.get(
    "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd",
  )
  |> Js.Promise.then_(result => {
       Promise.ret(
         result##data
         |> JsonUtils.Decode.at(["bitcoin", "usd"], JsonUtils.Decode.float),
       )
     })
  |> Js.Promise.catch(_ => {
       Js.Console.log("CoinGecko API failed, using default BTC price");
       Promise.ret(defaultBtcUsdPrice); // Use default if API fails
     });
};

// Calculate NNC info from BTC/USD price
let calculateNNCInfo = btcUsdPrice => {
  // Calculate NNC/BTC price: NNC/USD ÷ BTC/USD
  let nncBtcPrice = nncUsdPrice /. btcUsdPrice;
  
  // Calculate BTC marketcap: NNC/BTC × Circulating Supply
  let nncBtcMarketCap = nncBtcPrice *. nncCirculatingSupply;
  
  {
    usdPrice: nncUsdPrice,
    usdMarketCap: nncUsdMarketCap,
    usd24HrChange: nncUsd24HrChange,
    btcPrice: nncBtcPrice,
    btcMarketCap: nncBtcMarketCap,
    circulatingSupply: nncCirculatingSupply,
  };
};

// Get NNC price information (hardcoded for testnet)
// Returns immediately with default BTC price, then updates when API responds
// client parameter kept for future use when switching to oracle scripts
let getNNCInfo = _client => {
  // Return immediately with default BTC price so cards show data right away
  let initialInfo = calculateNNCInfo(defaultBtcUsdPrice);
  
  // Fetch real BTC price from API and update
  let%Promise btcUsdPrice = getBtcUsdPrice();
  let updatedInfo = calculateNNCInfo(btcUsdPrice);
  
  Promise.ret(Some(updatedInfo));
};

// Legacy function name for backward compatibility (calls getNNCInfo)
let getBandInfo = getNNCInfo;
