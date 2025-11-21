"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    Object.defineProperty(o, k2, { enumerable: true, get: function() { return m[k]; } });
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.Address = exports.PublicKey = exports.PrivateKey = void 0;
const bip39 = __importStar(require("bip39"));
const bip32 = __importStar(require("bip32"));
const bech32 = __importStar(require("bech32"));
const secp256k1_1 = __importDefault(require("secp256k1"));
const crypto_1 = __importDefault(require("crypto"));
const bitcoinjs_lib_1 = require("bitcoinjs-lib");
const error_1 = require("./error");
const keys_pb_1 = require("../proto/cosmos/crypto/secp256k1/keys_pb");
const BECH32_PUBKEY_ACC_PREFIX = 'bandpub';
const BECH32_PUBKEY_VAL_PREFIX = 'bandvaloperpub';
const BECH32_PUBKEY_CONS_PREFIX = 'bandvalconspub';
const BECH32_ADDR_ACC_PREFIX = 'band';
const BECH32_ADDR_VAL_PREFIX = 'bandvaloper';
const BECH32_ADDR_CONS_PREFIX = 'bandvalcons';
const DEFAULT_DERIVATION_PATH = "m/44'/494'/0'/0/0";
class PrivateKey {
    constructor(signingKey) {
        this.signingKey = signingKey;
    }
    static generate(path = DEFAULT_DERIVATION_PATH) {
        const phrase = bip39.generateMnemonic(256);
        return [phrase, this.fromMnemonic(phrase, path)];
    }
    static fromMnemonic(words, path = DEFAULT_DERIVATION_PATH) {
        const seed = bip39.mnemonicToSeedSync(words);
        const node = bip32.fromSeed(seed);
        const child = node.derivePath(path);
        if (!child.privateKey)
            throw new error_1.CreateError('Cannot create private key');
        const ecpair = bitcoinjs_lib_1.ECPair.fromPrivateKey(child.privateKey, {
            compressed: false,
        });
        if (!ecpair.privateKey)
            throw new error_1.CreateError('Cannot create private key');
        return new PrivateKey(ecpair.privateKey);
    }
    static fromHex(priv) {
        return new PrivateKey(Buffer.from(priv, 'hex'));
    }
    toHex() {
        return this.signingKey.toString('hex');
    }
    toPubkey() {
        const pubKeyByte = secp256k1_1.default.publicKeyCreate(this.signingKey);
        return PublicKey.fromHex(Buffer.from(pubKeyByte).toString('hex'));
    }
    sign(msg) {
        const hash = crypto_1.default.createHash('sha256').update(msg).digest('hex');
        const buf = Buffer.from(hash, 'hex');
        const { signature } = secp256k1_1.default.ecdsaSign(buf, this.signingKey);
        return Buffer.from(signature);
    }
}
exports.PrivateKey = PrivateKey;
class PublicKey {
    constructor(verifyKey) {
        this.verifyKey = verifyKey;
    }
    static fromBech32(bech, _prefix) {
        const { prefix, words } = bech32.decode(bech);
        if (prefix != _prefix)
            throw new error_1.ValueError('Invalid bech32 prefix');
        if (words.length === 0)
            throw new error_1.DecodeError('Cannot decode bech32');
        return new PublicKey(Buffer.from(bech32.fromWords(words).slice(5)));
    }
    static fromHex(pub) {
        return new PublicKey(Buffer.from(pub, 'hex'));
    }
    static fromAccBech32(bech) {
        return this.fromBech32(bech, BECH32_PUBKEY_ACC_PREFIX);
    }
    static fromValBech32(bech) {
        return this.fromBech32(bech, BECH32_PUBKEY_VAL_PREFIX);
    }
    static fromConsBech32(bech) {
        return this.fromBech32(bech, BECH32_PUBKEY_CONS_PREFIX);
    }
    toBech32(prefix) {
        const hex = Buffer.concat([
            Buffer.from('eb5ae98721', 'hex'),
            this.verifyKey,
        ]);
        const words = bech32.toWords(Buffer.from(hex));
        if (words.length === 0)
            throw new error_1.UnsuccessfulCallError('Unsuccessful bech32.toWords call');
        return bech32.encode(prefix, words);
    }
    toPubkeyProto() {
        const publicKeyProto = new keys_pb_1.PubKey();
        publicKeyProto.setKey(this.verifyKey);
        return publicKeyProto;
    }
    toAccBech32() {
        return this.toBech32(BECH32_PUBKEY_ACC_PREFIX);
    }
    toValBech32() {
        return this.toBech32(BECH32_PUBKEY_VAL_PREFIX);
    }
    toConsBech32() {
        return this.toBech32(BECH32_PUBKEY_CONS_PREFIX);
    }
    toHex() {
        return this.verifyKey.toString('hex');
    }
    toAddress() {
        const hash = crypto_1.default.createHash('sha256').update(this.verifyKey).digest();
        return Address.fromHex(crypto_1.default.createHash('ripemd160').update(hash).digest('hex'));
    }
    verify(msg, sig) {
        const hash = crypto_1.default.createHash('sha256').update(msg).digest('hex');
        const buf = Buffer.from(hash, 'hex');
        return secp256k1_1.default.ecdsaVerify(sig, buf, this.verifyKey);
    }
}
exports.PublicKey = PublicKey;
class Address {
    constructor(addr) {
        this.addr = addr;
    }
    static fromBech32(bech, _prefix) {
        const { prefix, words } = bech32.decode(bech);
        if (prefix != _prefix)
            throw new error_1.ValueError('Invalid bech32 prefix');
        if (words.length === 0)
            throw new error_1.DecodeError('Cannot decode bech32');
        return new Address(Buffer.from(bech32.fromWords(words)));
    }
    static fromHex(hex) {
        return new Address(Buffer.from(hex, 'hex'));
    }
    static fromAccBech32(bech) {
        return this.fromBech32(bech, BECH32_ADDR_ACC_PREFIX);
    }
    static fromValBech32(bech) {
        return this.fromBech32(bech, BECH32_ADDR_VAL_PREFIX);
    }
    static fromConsBech32(bech) {
        return this.fromBech32(bech, BECH32_ADDR_CONS_PREFIX);
    }
    toBech32(prefix) {
        const words = bech32.toWords(this.addr);
        if (words.length === 0)
            throw new error_1.UnsuccessfulCallError('Unsuccessful bech32.toWords call');
        return bech32.encode(prefix, words);
    }
    toAccBech32() {
        return this.toBech32(BECH32_ADDR_ACC_PREFIX);
    }
    toValBech32() {
        return this.toBech32(BECH32_ADDR_VAL_PREFIX);
    }
    toConsBech32() {
        return this.toBech32(BECH32_ADDR_CONS_PREFIX);
    }
    toHex() {
        return this.addr.toString('hex');
    }
}
exports.Address = Address;
