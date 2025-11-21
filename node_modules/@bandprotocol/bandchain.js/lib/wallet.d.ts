/// <reference types="node" />
import { PubKey as PublicKeyProto } from '../proto/cosmos/crypto/secp256k1/keys_pb';
export declare class PrivateKey {
    private signingKey;
    private constructor();
    static generate(path?: string): [string, PrivateKey];
    static fromMnemonic(words: string, path?: string): PrivateKey;
    static fromHex(priv: string): PrivateKey;
    toHex(): string;
    toPubkey(): PublicKey;
    sign(msg: Uint8Array): Buffer;
}
export declare class PublicKey {
    private verifyKey;
    private constructor();
    private static fromBech32;
    static fromHex(pub: string): PublicKey;
    static fromAccBech32(bech: string): PublicKey;
    static fromValBech32(bech: string): PublicKey;
    static fromConsBech32(bech: string): PublicKey;
    private toBech32;
    toPubkeyProto(): PublicKeyProto;
    toAccBech32(): string;
    toValBech32(): string;
    toConsBech32(): string;
    toHex(): string;
    toAddress(): Address;
    verify(msg: Buffer, sig: Buffer): boolean;
}
export declare class Address {
    private addr;
    private constructor();
    private static fromBech32;
    static fromHex(hex: string): Address;
    static fromAccBech32(bech: string): Address;
    static fromValBech32(bech: string): Address;
    static fromConsBech32(bech: string): Address;
    private toBech32;
    toAccBech32(): string;
    toValBech32(): string;
    toConsBech32(): string;
    toHex(): string;
}
