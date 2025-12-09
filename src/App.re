module Styles = {
  open Css;

  let container = (theme: Theme.t) =>
    style([
      width(`percent(100.)),
      position(`relative),
      overflow(`auto),
      backgroundColor(theme.mainBg),
    ]);

  let routeContainer =
    style([
      minHeight(`calc((`sub, `vh(100.), `px(193)))),
      Media.mobile([paddingBottom(`zero)]),
    ]);
};

[@react.component]
let make = () => {
  let currentRoute = ReasonReactRouter.useUrl() |> Route.fromUrl;
  let isMobile = Media.isMobile();
  let ({ThemeContext.theme}, _) = React.useContext(ThemeContext.context);

  <div className={Styles.container(theme)}>
    <Header />
    {isMobile
       ? <Section pt=16 pb=16 ptSm=24 pbSm=24>
           <div className=CssHelper.container> <SearchBar /> </div>
         </Section>
       : React.null}
    <div className=Styles.routeContainer>
      {switch (currentRoute) {
       | HomePage => <HomePage />
       | TxHomePage => <TxHomePage />
       | TxIndexPage(txHash) => <TxIndexPage txHash />
       | BlockHomePage => <BlockHomePage />
       | BlockIndexPage(height) => <BlockIndexPage height={ID.Block.ID(height)} />
       | ValidatorHomePage => <ValidatorHomePage />
       | ValidatorIndexPage(address, hashtag) => <ValidatorIndexPage address hashtag />
       | AccountIndexPage(address, hashtag) => <AccountIndexPage address hashtag />
       | NotFound => <NotFound />
       }}
    </div>
    <Footer />
    <Modal />
  </div>;
};
