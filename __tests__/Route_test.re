open Jest;
open Route;
open Expect;

describe("Expect Search Functionality to work correctly", () => {
  test("test block route", () =>
    expect("123" |> search) |> toEqual(BlockIndexPage(123))
  );
  test("test transaction route", () =>
    expect("22638794cb5f306ef929b90c58b27d26cb35a77ca5c5c624cf2025a98528c323" |> search)
    |> toEqual(
         TxIndexPage(
           "22638794cb5f306ef929b90c58b27d26cb35a77ca5c5c624cf2025a98528c323" |> Hash.fromHex,
         ),
       )
  );
  test("test transaction route prefix is 0x", () =>
    expect("22638794cb5f306ef929b90c58b27d26cb35a77ca5c5c624cf2025a98528c323" |> search)
    |> toEqual(
         TxIndexPage(
           "0x22638794cb5f306ef929b90c58b27d26cb35a77ca5c5c624cf2025a98528c323" |> Hash.fromHex,
         ),
       )
  );
  test("test block prefix is B", () =>
    expect("B123" |> search) |> toEqual(BlockIndexPage(123))
  );
  test("test block prefix is b", () =>
    expect("b123" |> search) |> toEqual(BlockIndexPage(123))
  );
  test("test data source route prefix is D returns NotFound", () =>
    expect("D123" |> search) |> toEqual(NotFound)
  );
  test("test data source route prefix is d returns NotFound", () =>
    expect("d123" |> search) |> toEqual(NotFound)
  );
  test("test request route prefix is R returns NotFound", () =>
    expect("R123" |> search) |> toEqual(NotFound)
  );
  test("test request route prefix is r returns NotFound", () =>
    expect("r123" |> search) |> toEqual(NotFound)
  );
  test("test oracle script route prefix is O returns NotFound", () =>
    expect("O123" |> search) |> toEqual(NotFound)
  );
  test("test oracle script route prefix is o returns NotFound", () =>
    expect("o123" |> search) |> toEqual(NotFound)
  );
  test("test validator route", () =>
    expect("nncvaloper1p40yh3zkmhcv0ecqp3mcazy83sa57rgjde6wec" |> search)
    |> toEqual(
         ValidatorIndexPage(
           "nncvaloper1p40yh3zkmhcv0ecqp3mcazy83sa57rgjde6wec" |> Address.fromBech32,
           Reports,
         ),
       )
  );
  test("test account route", () =>
    expect("nnc1p40yh3zkmhcv0ecqp3mcazy83sa57rgjp07dun" |> search)
    |> toEqual(
         AccountIndexPage(
           "nnc1p40yh3zkmhcv0ecqp3mcazy83sa57rgjp07dun" |> Address.fromBech32,
           AccountDelegations,
         ),
       )
  );
  test("test page not found", () =>
    expect("D123DD" |> search) |> toEqual(NotFound)
  );
});
