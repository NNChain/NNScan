module Styles = {
  open Css;

  let version = (theme: Theme.t, isDarkMode) =>
    style([
      display(`flex),
      borderRadius(`px(8)),
      border(`px(1), `solid, isDarkMode ? theme.secondaryBg : theme.textSecondary),
      backgroundColor(theme.secondaryBg),
      padding2(~v=`px(8), ~h=`px(10)),
      minWidth(`px(153)),
      justifyContent(`spaceBetween),
      alignItems(`center),
      position(`relative),
      cursor(`pointer),
      zIndex(5),
      Media.mobile([padding2(~v=`px(5), ~h=`px(10))]),
      Media.smallMobile([minWidth(`px(90))]),
    ]);

  let dropdown = (show, theme: Theme.t, isDarkMode) =>
    style([
      position(`absolute),
      width(`percent(100.)),
      border(`px(1), `solid, isDarkMode ? theme.secondaryBg : theme.textSecondary),
      backgroundColor(theme.secondaryBg),
      borderRadius(`px(8)),
      transition(~duration=200, "all"),
      top(`percent(110.)),
      left(`zero),
      height(`auto),
      opacity(show ? 1. : 0.),
      pointerEvents(show ? `auto : `none),
      overflow(`hidden),
      Media.mobile([top(`px(35))]),
    ]);

  let link = (theme: Theme.t) =>
    style([
      textDecoration(`none),
      backgroundColor(theme.secondaryBg),
      display(`block),
      padding2(~v=`px(5), ~h=`px(10)),
      hover([backgroundColor(theme.dropdownHover)]),
    ]);

  let linkDisabled = (theme: Theme.t) =>
    style([
      textDecoration(`none),
      backgroundColor(theme.secondaryBg),
      display(`block),
      padding2(~v=`px(5), ~h=`px(10)),
      opacity(0.5),
      cursor(`notAllowed),
      pointerEvents(`none),
    ]);
};

type chainID =
  | NNChainTestnet
  | NNChainMainnet;

let parseChainID =
  fun
  | chainID when Js.String.includes("testnet", chainID) || Js.String.includes("test", chainID) => NNChainTestnet
  | chainID when Js.String.includes("mainnet", chainID) || Js.String.includes("main", chainID) => NNChainMainnet
  | _ => NNChainTestnet; // Default to testnet

let getLink =
  fun
  | NNChainTestnet => "#" // Stay on current page for testnet
  | NNChainMainnet => "#"; // Stay on current page for mainnet (disabled)

let getName =
  fun
  | NNChainTestnet => "NNChain - Testnet"
  | NNChainMainnet => "NNChain - Mainnet";

[@react.component]
let make = () =>
  {
    let (show, setShow) = React.useState(_ => false);
    let trackingSub = TrackingSub.use();
    let ({ThemeContext.theme, isDarkMode}, _) = React.useContext(ThemeContext.context);

    let%Sub tracking = trackingSub;
    let currentChainID = tracking.chainID->parseChainID;

    <div
      className={Styles.version(theme, isDarkMode)}
      onClick={event => {
        setShow(oldVal => !oldVal);
        ReactEvent.Mouse.stopPropagation(event);
      }}>
      <Text
        value={currentChainID->getName}
        color={theme.textPrimary}
        nowrap=true
        weight=Text.Semibold
      />
      <HSpacing size=Spacing.sm />
      {show
         ? <Icon name="far fa-angle-up" color={theme.textSecondary} />
         : <Icon name="far fa-angle-down" color={theme.textSecondary} />}
      <div className={Styles.dropdown(show, theme, isDarkMode)}>
        {switch (currentChainID) {
         | NNChainTestnet =>
           [|NNChainMainnet|]
           ->Belt.Array.map(chainID => {
               let name = chainID->getName;
               <div key=name className={Styles.linkDisabled(theme)}>
                 <Text value=name color={theme.textSecondary} nowrap=true weight=Text.Semibold />
               </div>;
             })
           ->React.array
         | NNChainMainnet => React.null
         }}
      </div>
    </div>
    |> Sub.resolve;
  }
  |> Sub.default(
       _,
       {
         let width = Media.isSmallMobile() ? 80 : 153;
         <LoadingCensorBar width height=30 />;
       },
     );
