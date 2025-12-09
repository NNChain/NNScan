type t = {
  chainID: string,
  replayOffset: int,
};

module Config = [%graphql
  {|
  subscription Tracking {
    tracking @bsRecord {
      chainID: chain_id
      replayOffset: replay_offset
    }
  }
|}
];

let use = () => {
  let (result, _) = ApolloHooks.useSubscription(Config.definition);
  result
  |> Sub.map(_, internal => {
       switch (internal##tracking->Belt.Array.get(0)) {
       | Some(tracking) => tracking
       | None => {
           chainID: "",
           replayOffset: 0,
         }
       }
     });
};
