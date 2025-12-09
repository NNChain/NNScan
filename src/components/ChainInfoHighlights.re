module Styles = {
  open Css;

  let card = (theme: Theme.t) =>
    style([
      backgroundColor(theme.secondaryBg),
      position(`relative),
      borderRadius(`px(8)),
      boxShadow(Shadow.box(~x=`zero, ~y=`px(2), ~blur=`px(4), Css.rgba(0, 0, 0, `num(0.2)))),
      Media.smallMobile([margin2(~v=`zero, ~h=`px(-5))]),
    ]);

  let innerCard =
    style([
      position(`relative),
      zIndex(2),
      minHeight(`px(177)),
      padding2(~v=`px(24), ~h=`px(32)),
      Media.mobile([padding2(~v=`px(10), ~h=`px(12)), minHeight(`px(146))]),
    ]);

  let fullWidth = style([width(`percent(100.))]);

  let specialBg = (isDarkMode: bool) =>
    style([
      backgroundImage(
        `linearGradient((
          `deg(270.),
          isDarkMode
            ? [(`percent(0.), hex("58595B")), (`percent(100.), hex("231F20"))]
            : [(`percent(0.), hex("F5F5F5")), (`percent(100.), hex("E5E5E5"))],
        )),
      ),
    ]);

  let bandToken = (isDarkMode: bool) =>
    style([
      position(`absolute),
      width(`px(140)),
      top(`px(-35)),
      right(`px(-15)),
      zIndex(1),
      opacity(1.0),
      filter([
        `dropShadow((`px(0), `px(0), `px(8), Css.rgba(255, 255, 255, isDarkMode ? `num(0.4) : `num(0.25)))),
        `dropShadow((`px(0), `px(0), `px(16), Css.rgba(255, 255, 255, isDarkMode ? `num(0.3) : `num(0.2)))),
        `dropShadow((`px(0), `px(0), `px(24), Css.rgba(255, 255, 255, isDarkMode ? `num(0.2) : `num(0.15)))),
      ]),
      transform(rotateY(`deg(5.))),
      transition(~duration=200, ~timingFunction=`easeOut, "all"),
      Media.mobile([width(`px(110)), top(`px(-30)), right(`px(-10))]),
    ]);
};

module HighlightCard = {
  [@react.component]
  let make =
      (~label, ~valueAndExtraComponentSub: ApolloHooks.Subscription.variant(_), ~special=false) => {
    let (ThemeContext.{theme, isDarkMode}, _) = React.useContext(ThemeContext.context);
    let isMobile = Media.isMobile();

    <div className={Css.merge([Styles.card(theme), special ? Styles.specialBg(isDarkMode) : ""])}>
      {special && !isMobile
         ? <img alt="NNC Coin" src=Images.nncCoin className={Styles.bandToken(isDarkMode)} /> : React.null}
      <div
        id={"highlight-" ++ label}
        className={Css.merge([
          Styles.innerCard,
          CssHelper.flexBox(~direction=`column, ~justify=`spaceBetween, ~align=`flexStart, ()),
        ])}>
        {switch (valueAndExtraComponentSub) {
         | Data((valueComponent, extraComponent)) =>
           <> <Text value=label size=Text.Lg /> valueComponent extraComponent </>
         | _ =>
           <>
             <LoadingCensorBar width=90 height=18 />
             <LoadingCensorBar width=120 height=20 />
             <LoadingCensorBar width=75 height=15 />
           </>
         }}
      </div>
    </div>;
  };
};

[@react.component]
let make = (~latestBlockSub: Sub.t(BlockSub.t)) => {
  let infoSub = React.useContext(GlobalContext.context);
  let (ThemeContext.{theme, isDarkMode}, _) = React.useContext(ThemeContext.context);
  let activeValidatorCountSub = ValidatorSub.countByActive(true);
  let bondedTokenCountSub = ValidatorSub.getTotalBondedAmount();

  let validatorInfoSub = Sub.all2(activeValidatorCountSub, bondedTokenCountSub);
  let allSub = Sub.all3(latestBlockSub, infoSub, validatorInfoSub);

  <Row justify=Row.Between>
    <Col col=Col.Three colSm=Col.Six mbSm=16>
      // ORIGINAL BLOCK - Commented out (fetches real data from API)
      // <HighlightCard
      //   label="NNC Price"
      //   special=true
      //   valueAndExtraComponentSub={
      //     let%Sub (_, {financial}, _) = allSub;
      //     (
      //       {
      //         let bandPriceInUSD = "$" ++ (financial.usdPrice |> Format.fPretty(~digits=2));
      //         <Text
      //           value=bandPriceInUSD
      //           size=Text.Xxxl
      //           weight=Text.Semibold
      //           color={theme.white}
      //         />;
      //       },
      //       {
      //         let bandPriceInBTC = financial.btcPrice;
      //
      //         <div
      //           className={Css.merge([
      //             CssHelper.flexBox(~justify=`spaceBetween, ()),
      //             Styles.fullWidth,
      //           ])}>
      //           <Text value={bandPriceInBTC->Format.fPretty ++ " BTC"} />
      //         </div>;
      //       },
      //     )
      //     |> Sub.resolve;
      //   }
      // />
      
      <HighlightCard
        label="NNC Price"
        special=true
        valueAndExtraComponentSub={
          let%Sub (_, {financial}, _) = allSub;
          let (ThemeContext.{theme, isDarkMode}, _) = React.useContext(ThemeContext.context);
          (
            {
              let nncPriceInUSD = "$" ++ (financial.usdPrice |> Format.fPretty(~digits=2));
              <Text
                value=nncPriceInUSD
                size=Text.Xxxl
                weight=Text.Semibold
                color={isDarkMode ? theme.white : theme.textPrimary}
              />;
            },
            {
              let nncPriceInBTC = financial.btcPrice;
              <div
                className={Css.merge([
                  CssHelper.flexBox(~justify=`spaceBetween, ()),
                  Styles.fullWidth,
                ])}>
                <Text 
                  value={(nncPriceInBTC |> Format.fPretty) ++ " BTC"} 
                  color={isDarkMode ? theme.white : theme.textPrimary}
                />
              </div>;
            },
          )
          |> Sub.resolve;
        }
      />
    </Col>
    <Col col=Col.Three colSm=Col.Six mbSm=16>
      <HighlightCard
        label="Market Cap"
        valueAndExtraComponentSub={
          let%Sub (_, {financial}, _) = allSub;
          (
            {
              <Text
                value={"$" ++ (financial.usdMarketCap |> Format.fCurrency)}
                size=Text.Xxxl
                color={theme.textPrimary}
                weight=Text.Semibold
              />;
            },
            {
              let marketcap = financial.btcMarketCap;
              <Text value={(marketcap |> Format.fPretty) ++ " BTC"} />;
            },
          )
          |> Sub.resolve;
        }
      />
    </Col>
    <Col col=Col.Three colSm=Col.Six>
      <HighlightCard
        label="Latest Block"
        valueAndExtraComponentSub={
          let%Sub ({height, validator: {moniker, identity, operatorAddress}}, _, _) = allSub;
          (
            <TypeID.Block id=height position=TypeID.Landing />,
            <ValidatorMonikerLink
              validatorAddress=operatorAddress
              moniker
              identity
              width={`percent(100.)}
              avatarWidth=20
            />,
          )
          |> Sub.resolve;
        }
      />
    </Col>
    <Col col=Col.Three colSm=Col.Six>
      <HighlightCard
        label="Active Validators"
        valueAndExtraComponentSub={
          let%Sub (_, _, (activeValidatorCount, bondedTokenCount)) = allSub;
          (
            {
              let activeValidators = activeValidatorCount->Format.iPretty ++ " Nodes";
              <Text
                value=activeValidators
                size=Text.Xxxl
                color={theme.textPrimary}
                weight=Text.Semibold
              />;
            },
            <Text
              value={
                (bondedTokenCount |> Coin.getBandAmountFromCoin |> Format.fPretty)
                ++ " NNC Bonded"
              }
            />,
          )
          |> Sub.resolve;
        }
      />
    </Col>
  </Row>;
};
