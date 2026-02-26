using Microsoft.EntityFrameworkCore.Metadata.Internal;
using Microsoft.EntityFrameworkCore.Storage.ValueConversion;
using Microsoft.EntityFrameworkCore.Update.Internal;
using Microsoft.VisualBasic;
using SQLitePCL;
using System;
using System.Collections;
using System.Collections.Generic;
using System.DirectoryServices.ActiveDirectory;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Security.Policy;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace Cyclon
{
    partial class Word
    {
        public override Obj SelfC(Local local)
        {
            var obj = local.getC(name, local).SelfC(local);
            obj.letter = letter;
            return obj;
        }
        public override Obj GetterC(Local local)
        {
            var obj = local.getC(name, local).SelfC(local).GetterC(local);
            obj.letter = letter;
            return obj;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            n++;
            return this;
        }
    }
    partial class Value
    {
        public Value(Type cls, Vari vari) : base(ObjType.Value)
        {
            this.cls = cls;
            this.vari = vari;
            opesC["+"] = opesC["-"] = opesC["*"] = opesC["/"] = opesC["=="] = opesC["!="] = OpeC;
            opesC["<"] = opesC[">"] = opesC["<="] = opesC[">="] = OpeC;
        }
        public Obj OpeC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Value)
            {
                var value = val2 as Value;
                if (cls == local.Int  || cls == local.Short)
                {
                    var sizetype = "i32";
                    if (cls == local.Short) sizetype = "i16";
                    if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                    if (value.cls == local.Str)
                    {
                        if (op == "+")
                        {
                            if (sizetype != "i32")
                            {
                                var v = new Vari("i32", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, this.vari));
                                this.vari = v;
                            }
                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                            var nsvari = new Vari("%NumberStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(nsvari, new Vari("%NumberStringType*", "@NumberString")));
                            var v0 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v0, nsvari, thgc4, this.vari));
                            var as2vari = new Vari("%AddStringType2*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(as2vari, new Vari("%AddStringType2*", "@AddString2")));
                            var v1 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v1, as2vari, thgc4, v0, value.vari));
                            return new Value(local.Str, v1);
                        }
                        else return Obj.Error(ObjType.Error, letter, "値の演算は" + op + "がサポートされていません。");
                    }
                    else if (value.cls == local.Int || value.cls == local.Short)
                    {
                        Type ret = cls;
                        if (cls != value.cls)
                        {
                            sizetype = "i32";
                            if (value.cls == local.Short)
                            {
                                var v = new Vari("i32", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, value.vari));
                                value.vari = v;
                                ret = local.Int;
                            }
                            else if (cls == local.Short)
                            {
                                var v = new Vari("i32", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, this.vari));
                                this.vari = v;
                                ret = local.Int;
                            }
                        }
                        Vari vari = new Vari(sizetype, "%v" + LLVM.n++);
                        if (op == "+") local.llvm.func.comps.Add(new Add(vari, this.vari, val2.vari));
                        else if (op == "-") local.llvm.func.comps.Add(new Sub(vari, this.vari, val2.vari));
                        else if (op == "*") local.llvm.func.comps.Add(new Mul(vari, this.vari, val2.vari));
                        else if (op == "/") local.llvm.func.comps.Add(new SDiv(vari, this.vari, val2.vari));
                        else if (op == "%") local.llvm.func.comps.Add(new SRem(vari, this.vari, val2.vari));
                        else if (op == "==")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new Eq(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == "!=")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new Ne(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == "<")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SLessThan(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == ">")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SMoreThan(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == "<=")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SLessEqual(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == ">=")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SMoreEqual(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else return Obj.Error(ObjType.Error, letter, "値の演算は" + op + "がサポートされていません。");
                        return new Value(ret, vari);
                    }
                }
                else if (cls == local.Bool)
                {
                }
                else if (cls == local.Str)
                {
                    if (value.cls == local.Int)
                    {
                        if (value.cls == local.Short)
                        {
                            var v = new Vari("i32", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Bitcast(v, value.vari));
                            value.vari = v;
                        }
                        if (op == "+")
                        {
                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                            var nsvari = new Vari("%NumberStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(nsvari, new Vari("%NumberStringType*", "@NumberString")));
                            var v0 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v0, nsvari, thgc4, value.vari));
                            var as2vari = new Vari("%AddStringType2*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(as2vari, new Vari("%AddStringType2*", "@AddString2")));
                            var v1 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v1, as2vari, thgc4, this.vari, v0));
                            return new Value(local.Str, v1);
                        }
                        else return Obj.Error(ObjType.Error, letter, "値の演算は" + op + "がサポートされていません。");
                    }
                    else if (value.cls == local.Str)
                    {
                        if (op == "+")
                        {
                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                            var as2vari = new Vari("%AddStringType2*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(as2vari, new Vari("%AddStringType2*", "@AddString2")));
                            var v1 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v1, as2vari, thgc4, this.vari, value.vari));
                            return new Value(local.Str, v1);
                        }
                        else if (op == "==")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new Eq(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == "!=")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new Ne(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == "<")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SLessThan(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == ">")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SMoreThan(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == "<=")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SLessEqual(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else if (op == ">=")
                        {
                            vari.type = "i1";
                            local.llvm.func.comps.Add(new SMoreEqual(vari, this.vari, val2.vari));
                            return new Value(local.Bool, vari);
                        }
                        else return Obj.Error(ObjType.Error, letter, "値の演算は" + op + "がサポートされていません。");
                    }
                    else return Obj.Error(ObjType.Error, letter, "値の演算は" + op + "がサポートされていません。");
                }
            }
            return Obj.Error(ObjType.Error, letter, "値の演算は値同士でなければなりません。");
        }
        public override Obj ope(string key, Local local, Obj val2)
        {
            Vari vari = null;
            Vari vari2 = null;
            if (cls == local.Int)
            {
                vari = new Vari("i32*", "%v" + LLVM.n++);
            }
            else if (cls == local.Bool)
            {
                vari = new Vari("i2*", "%v" + LLVM.n++);
            }
            local.llvm.func.comps.Add(new Load(vari, this.vari));
            var val = new Value(cls, vari2);
            if (key == "+") local.llvm.func.comps.Add(new Add(vari2, vari, val2.vari));
            else if (key == "-") local.llvm.func.comps.Add(new Sub(vari2, vari, val2.vari));
            else if (key == "*") local.llvm.func.comps.Add(new Mul(vari2, vari, val2.vari));
            else if (key == "/") local.llvm.func.comps.Add(new SDiv(vari2, vari, val2.vari));
            else if (key == "%") local.llvm.func.comps.Add(new SRem(vari2, vari, val2.vari));
            else if (key == "==")
            {
                vari2.type = "i1";
                local.llvm.func.comps.Add(new Eq(vari2, vari, val2.vari));
            }
            else if (key == "!=")
            {
                vari2.type = "i1";
                local.llvm.func.comps.Add(new Ne(vari2, vari, val2.vari));
            }
            else if (key == ">")
            {
                vari2.type = "i1";
                local.llvm.func.comps.Add(new SMoreThan(vari2, vari, val2.vari));
            }
            else if (key == "<")
            {
                vari2.type = "i1";
                local.llvm.func.comps.Add(new SLessThan(vari2, vari, val2.vari));
            }
            else if (key == ">=")
            {
                vari2.type = "i1";
                local.llvm.func.comps.Add(new SMoreEqual(vari2, vari, val2.vari));
            }
            else if (key == "<=")
            {
                vari2.type = "i1";
                local.llvm.func.comps.Add(new SLessEqual(vari2, vari, val2.vari));
            }
            return val;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    n++;
                    var name = (val2 as Word).name;
                    var vmap0 = ((cls as ClassObj).draw.children[0] as Block).vmapA;
                    var vmap = ((cls as ClassObj).draw.children[1] as Block).vmapA;
                    if (vmap0.ContainsKey(name))
                    {
                        if (vmap0[name].type == ObjType.Variable)
                        {
                            var variable = vmap0[name] as Variable;
                            Vari vari;
                            if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                            if (variable.cls == local.Int) vari = new Vari("i32*", "%v" + LLVM.n++);
                            else if (variable.cls == local.Bool) vari = new Vari("i2*", "%v" + LLVM.n++);
                            else vari = new Vari(variable.cls.model, "%v" + LLVM.n++);
                            var gete = new Gete(cls.model, vari, this.vari, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            local.llvm.func.comps.Add(gete);
                            variable = variable.Clone() as Variable;
                            variable.vari = vari;
                            return variable;
                        }
                        else if (vmap[name].type == ObjType.Function)
                        {
                            var func = vmap[name] as Function;
                            Vari vari;
                            if (func.ret.type == ObjType.Var) func.ret = (func.ret as Var).cls;
                            if (func.ret == local.Int) vari = new Vari("i32*", "%v" + LLVM.n++);
                            else if (func.ret == local.Bool) vari = new Vari("i2*", "%v" + LLVM.n++);
                            else vari = new Vari(func.ret.model, "%v" + LLVM.n++);
                            var gete = new Gete(cls.model, vari, this.vari, new Vari("i32", "0"), new Vari("i32", func.order.ToString()));
                            local.llvm.func.comps.Add(gete);
                            local.llvm.func.vari = vari;
                            return func;
                        }
                    }
                    else if (vmap.ContainsKey(name))
                    {
                        if (vmap[name].type == ObjType.Variable)
                        {
                            var variable = vmap[name] as Variable;
                            Vari vari;
                            if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                            if (variable.cls == local.Int) vari = new Vari("i32*", "%v" + LLVM.n++);
                            else if (variable.cls == local.Bool) vari = new Vari("i2*", "%v" + LLVM.n++);
                            else vari = new Vari(variable.cls.model, "%v" + LLVM.n++);
                            var gete = new Gete(cls.model, vari, this.vari, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            local.llvm.func.comps.Add(gete);
                            variable = variable.Clone() as Variable;
                            variable.vari = vari;
                            return variable;
                        }
                        else if (vmap[name].type == ObjType.Function)
                        {
                            var func = vmap[name] as Function;
                            Vari vari;
                            if (func.ret.type == ObjType.Var) func.ret = (func.ret as Var).cls;
                            if (func.ret == local.Int) vari = new Vari("i32*", "%v" + LLVM.n++);
                            else if (func.ret == local.Bool) vari = new Vari("i2*", "%v" + LLVM.n++);
                            else vari = new Vari(func.ret.model, "%v" + LLVM.n++);
                            var gete = new Gete(cls.model, vari, this.vari, new Vari("i32", "0"), new Vari("i32", func.order.ToString()));
                            local.llvm.func.comps.Add(gete);
                            local.llvm.func.vari = vari;
                            return func;
                        }
                    }
                }
            }
            else if (val2.type == ObjType.Block)
            {
                n++;
                var val = val2.exeC(local).GetterC(local) as Block;
                if (cls.type == ObjType.Var) cls = (cls as Var).cls;
                if (cls == local.Str)
                {
                    this.vari.type = "s[n]";
                    var value = val.rets[0] as Value;
                    if (value.cls == local.Int)
                    {
                        var variable = new Variable(local.Short, this.vari);
                        variable.strvari = value.vari;
                        return variable;
                    }
                    else return Obj.Error(ObjType.Error, letter, "文字列のブロックは整数型をとるしかありません。");
                }
                if (val.rets[0].type == ObjType.Value)
                {
                    var value = val.rets[0] as Value;
                    if (value.cls == local.Int)
                    {
                        var v0 = new Vari("%ListType**", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%HashType", v0, this.vari, new Vari("i32", "0"), new Vari("i32", "0")));
                        var v05 = new Vari("%ListType*", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(v05, v0));
                        var varigl = new Vari("%GetListType", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(varigl, new Vari("%GetListType*", "@GetList")));
                        var v1 = new Vari("%KeyValueType**", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Call(v1, varigl, v05, value.vari));
                        var v15 = new Vari("%KeyValueType*", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(v15, v1));
                        var v2 = new Vari("[n]", " %v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%KeyValueType", v2, v15, new Vari("i32", "0"), new Vari("i32", "2")));
                        this.vari = v2;
                        if (cls.type == ObjType.ArrayType)
                        {
                            var arrtype = cls as ArrType;
                            return new Variable(arrtype.cls, this.vari);
                        }
                        else return Obj.Error(ObjType.Error, letter, "ブロックは配列型をとるしかありません。");
                    }
                    else if (value.cls == local.Str)
                    {
                        this.vari.type = "[s]";
                        if (cls.type == ObjType.Var) cls = (cls as Var).cls;
                        if (cls.type == ObjType.ArrayType)
                        {
                            var arrtype = cls as ArrType;
                            var variable = new Variable(arrtype.cls, this.vari);
                            variable.strvari = value.vari;
                            return variable;
                        }
                        else return Obj.Error(ObjType.Error, letter, "ブロックは配列型をとるしかありません。");
                    }
                    else return Obj.Error(ObjType.Error, val.letters[0],  "配列の引数は整数か文字列だけです。");
                }
                return Obj.Error(ObjType.Error, val.letters[0], "配列の引数は整数か文字列だけです。");
            }
            return Obj.Error(ObjType.NG, val2.letter, "値の後続が適切ではないです");
        }
    }
    partial class Class
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Block)
                {
                    var generic = local.getC(word.name, local) as Generic;
                    var block = val2.exeC(local) as Block;
                    local.blocks.Add(block);
                    for (var i = 0; i < block.rets.Count; i++)
                    {
                        val2 = block.rets[i];
                    }
                    local.blocks.RemoveAt(local.blocks.Count - 1);
                    generic.block = block;
                    n++;
                    val2 = primary.children[n];
                }
            }
        head:
            if (val2.type == ObjType.Left)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Bracket)
                {
                    var val = primary.children[n].exeC(local).GetterC(local);
                    if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
                    var blk = val2 as Block;
                    n++;
                    val2 = primary.children[n];
                    for (var i = 0; i < blk.rets.Count; i++)
                    {
                        if (blk.rets[i].type == ObjType.ClassObj || blk.rets[i].type == ObjType.GenericObj)
                        {
                            var cls = val2 as Type;
                            goto head;
                        }
                        else return Obj.Error(ObjType.Error, blk.letters[i], "クラスの継承元はクラスでなければなりません");
                    }
                }
                else
                {
                    val2 = val2.GetterC(local);
                    if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
                    if (val2.type == ObjType.ClassObj)
                    {
                        var cls = val2 as ClassObj;
                        n++;
                        val2 = primary.children[n];
                        goto head;
                    }
                    else if (val2.type == ObjType.Generic)
                    {
                        var generic = val2 as Generic;
                        n++;
                        if (val2.type == ObjType.Block)
                        {
                            val2 = primary.children[n].exeC(local).GetterC(local);
                            if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
                            var block = val2 as Block;
                            var genericobj = new GenericObj(generic);
                            for (var i = 0; i < block.rets.Count; i++)
                            {
                                val2 = block.rets[i];
                                if (val2 is Type)
                                {
                                    genericobj.draws.Add(val2 as Type);
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "ジェネリックスクラスの引数は型でなければならない");
                            }
                            n++;
                            val2 = primary.children[n];
                            if (generic.vmap.Count != genericobj.draws.Count) return Obj.Error(ObjType.Error, block.letter, "ジェネリックスクラスの引数の数が違います");
                            goto head;
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "ジェネリッククラスは[]で引数をもたなければならないです");
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "クラスの継承元はクラスでなければなりません");
                }
            }
            if (val2.type == ObjType.CallBlock)
            {
                n++;
                var obj = local.getC2(val2 as CallBlock);
                return obj;
            }
            return Obj.Error(ObjType.NG, val2.letter, "クラスの宣言の{|}が抜けています");
        }

    }
    partial class ArrType
    {
        public override string model
        {
            get
            {
                return "%HashType";
            }
        }
    }
    partial class FuncType
    {
        public override string model {
            get {
                return "%HashType";
            }
        }
    }
    partial class ClassObj
    {
        public override string model
        {
            get { return "%" + this.letter.name + "Type" + identity; }
        }
        public override string call
        {
            get { return "@" + this.letter.name + identity; }
        }
        public override string drawcall
        {
            get { return "@" + this.letter.name + "Draw" + identity; }
        }
        public override void Dec(Local local)
        {
            if (identity == 0) identity = LLVM.n++;
            ifv = new Dictionary<string, IfValue>();
            this.n = Obj.cn++;
            this.vari = new Vari("void", call);
            var rn4 = new Vari("%RootNodeType*", "%rn");
            var obj = new Vari(this.model + "*", "%obj");
            var func = new Func(local.llvm, this.vari, rn4, obj);
            local.llvm.comps.Add(func);
            local.llvm.funcs.Add(func);
            var thgcptr4 = new Vari("%ThreadGCType**", "%thgcptr");
            func.comps.Add(new Gete("%RootNodeType", thgcptr4, rn4, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            func.comps.Add(new Load(thgc4, thgcptr4));

            var objptr = new Vari(this.model + "**", "%objptr");
            var alloca = new Alloca(objptr);
            func.comps.Add(alloca);
            var objstore = new Store(objptr, obj);
            func.comps.Add(objstore);
            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            func.comps.Add(srload);
            var srcall = new Call(null, srv, rn4, objptr);
            func.comps.Add(srcall);


            var rn5 = new Vari("%RootNodeType*", "%rn");
            var blk = new Vari("i8*", "%fptr1");
            var funcdraw = new Func(local.llvm, new Vari(this.model + "*", drawcall), rn5, blk);
            local.llvm.comps.Add(funcdraw);

            var typedec = new TypeDec(this.model);
            local.llvm.types.Add(typedec);
            typedec.comps.Add(new TypeVal("i8*", "blk"));

            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var checkname = "@" + this.letter.name + "Check" + identity;
            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            var countv = new Vari("i32", "0");
            var strv = new StrV("@" + this.letter.name + identity, this.letter.name, this.letter.name.Length * 1);
            local.llvm.strs.Add(strv);
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var thgcptr5 = new Vari("%ThreadGCType**", "%thgcptr");
            funcdraw.comps.Add(new Gete("%RootNodeType", thgcptr5, rn5, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc5 = new Vari("%ThreadGCType*", "%thgc");
            funcdraw.comps.Add(new Load(thgc5, thgcptr5));
            var go_val = new Vari("%CopyObjectType", "%gcobject");
            var go_load = new Load(go_val, new Vari("%CopyObjectType*", "@CloneObject"));
            funcdraw.comps.Add(go_load);
            var v3 = new Vari("i8**", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", v3, blk, new Vari("i32", "0"), new Vari("i32", "3"));
            funcdraw.comps.Add(gete);
            var v4 = new Vari("i8*", "%v" + LLVM.n++);
            funcdraw.comps.Add(new Load(v4, v3));
            var go_v = new Vari("i8*", "%obj");
            var go_call = new Call(go_v, go_val, thgc5, v4);
            funcdraw.comps.Add(go_call);
            var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
            var go_cast = new Bitcast(go_c, go_v);
            funcdraw.comps.Add(go_cast);

            var rn6 = new Vari("%RootNodeType*", "%rn");
            var block = new Vari("i8*", "%block");
            var funcptr = new Vari("i8*", "%fptr");
            var funcptr2 = new Vari("i8*", "%fptr2");
            var decname = "@" + this.letter.name + "dec" + identity;
            var funcdec = new Func(local.llvm, new Vari("%FuncType*", decname), rn6, block, funcptr, funcptr2);
            local.llvm.comps.Add(funcdec);
            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
            funcdec.comps.Add(new Load(thgc6, new Vari("%ThreadGCType**", "@thgcp")));

            var gmvari = new Vari("%GC_mallocType", "%gm");
            var gmload = new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc"));
            funcdec.comps.Add(gmload);
            var tmp = new Vari("i32", 28.ToString());
            go_call = new Call(go_v, gmvari, thgc6, tmp);
            funcdec.comps.Add(go_call);
            var go_c3 = new Vari("%FuncType*", "%v" + LLVM.n++);
            go_cast = new Bitcast(go_c3, go_v);
            funcdec.comps.Add(go_cast);

            var vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc, go_c3, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);
            var vc2 = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc2, go_c3, new Vari("i32", "0"), new Vari("i32", "1"));
            funcdec.comps.Add(gete);
            store = new Store(vc2, funcptr);
            funcdec.comps.Add(store);
            var vc3 = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc3, go_c3, new Vari("i32", "0"), new Vari("i32", "2"));
            funcdec.comps.Add(gete);
            store = new Store(vc3, funcptr2);
            funcdec.comps.Add(store);

            var objptr2 = new Vari("%FuncType**", "%objptr");
            var alloca2 = new Alloca(objptr2);
            funcdec.comps.Add(alloca2);
            var objstore2 = new Store(objptr2, go_c3);
            funcdec.comps.Add(objstore2);

            srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            funcdec.comps.Add(srload);
            srcall = new Call(null, srv, rn6, objptr2);
            funcdec.comps.Add(srcall);


            go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
            tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            funcdec.comps.Add(new Add(tmv, tmi, tmp));
            go_call = new Call(go_v, gmvari, thgc6, tmv);
            funcdec.comps.Add(go_call);
            var go_c2 = new Vari(this.model + "*", "%v" + LLVM.n++);
            go_cast = new Bitcast(go_c2, go_v);
            funcdec.comps.Add(go_cast);

            vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete(this.model, vc, go_c2, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);

            var objptr3 = new Vari(this.model + "**", "%v" + LLVM.n++);
            var alloca3 = new Alloca(objptr3);
            funcdec.comps.Add(alloca3);
            var objstore3 = new Store(objptr3, go_c2);
            funcdec.comps.Add(objstore3);

            srcall = new Call(null, srv, thgc6, objptr3);
            funcdec.comps.Add(srcall);

            var vc4 = new Vari(this.model + "**", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc4, go_c3, new Vari("i32", "0"), new Vari("i32", "3"));
            funcdec.comps.Add(gete);
            var parentstore = new Store(vc4, go_c2);
            funcdec.comps.Add(parentstore);

            int count = 8;
            var blk0 = draw.children[0] as Block;
            int order = 1;
            var blk2 = draw.children[1] as Block;
            var decs = new List<Obj>();
            foreach (var kv in blk0.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        var vdraw = new Vari("i32", "%v" + LLVM.n++);
                        funcdraw.draws.Add(vdraw);
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;

                        var varii = new Vari("i32*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);
                    }
                    else if (variable.cls == local.Bool)
                    {
                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                        funcdraw.draws.Add(new Vari("i32", "%v" + LLVM.n++));
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;

                        var varii = new Vari("i1*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        funcdraw.draws.Add(vdraw);
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        var varii = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    funcdraw.draws.Add(new Vari(f.model + "*", "%v" + LLVM.n++));
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                    funcdraw.draws.Add(new Vari("%FuncType*", "%v" + LLVM.n++));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);
                }
            }
            foreach (var kv in blk2.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    decs.Add(f);
                    f.order = order++;
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);
                }
            }
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            local.llvm.funcs.Add(funcdec);
            this.bas = objptr2;
            local.blocks.Add(blk0);
            blk0.exeC(local);
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            local.blocks.Add(blk2);

            funcdraw.comps.Add(new Ret(go_c));
            countv.name = count.ToString();
            funccheck.comps.Add(new Ret(new Vari("void", null)));
            var vv = new Vari("%FuncType*", "%v" + LLVM.n++);
            funcdec.comps.Add(new Load(vv, objptr2));

            var rnpv = new Vari("i32*", "%ptr");
            funcdec.comps.Add(new Gete("%RootNodeType", rnpv, rn6, new Vari("i32", "0"), new Vari("i32", "1")));
            var rnpv12 = new Vari("i32", "%v");
            var rnp12load = new Load(rnpv12, rnpv);
            funcdec.comps.Add(rnp12load);
            var rnpv2 = new Vari("i32", "%dec");
            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            funcdec.comps.Add(rnpsub);
            funcdec.comps.Add(new Store(rnpv, rnpv2));
            funcdec.comps.Add(new Ret(go_c3));

            this.bas = objptr;
            local.labs.Add(new Lab("entry"));
            blk2.exeC(local);
            local.labs.RemoveAt(local.labs.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);

            rnpv = new Vari("i32*", "%ptr");
            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
            rnpv12 = new Vari("i32", "%v");
            rnp12load = new Load(rnpv12, rnpv);
            local.llvm.func.comps.Add(rnp12load);
            rnpv2 = new Vari("i32", "%dec");
            rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            local.llvm.func.comps.Add(rnpsub);
            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));

            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
        }
    }
    partial class Type
    {
        public int identity;
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            Type type = this;
        head:
            if (val2.type == ObjType.Block)
            {
                var blk = val2 as Block;
                if (blk.children.Count == 0)
                {
                    type = new ArrType(type) { letter = val2.letter };
                    n++;
                    val2 = primary.children[n];
                    goto head;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "配列型は引数の数が0です");
            }
            else if (val2.type == ObjType.Left)
            {
                var functype = new FuncType(type);
                type = functype;
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Bracket)
                {
                    local.calls.Add(local.KouhoSetType);
                    val2 = val2.exeC(local).GetterC(local);
                    local.calls.RemoveAt(local.calls.Count - 1);
                    if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
                    var blk = val2 as Block;
                    for (var i = 0; i < blk.rets.Count; i++)
                    {
                        val2 = blk.rets[i];
                        if (val2 is Type)
                        {
                            functype.draws.Add(val2 as Type);
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "継承元はクラスでなければならない"); ;
                    }
                    n++;
                    val2 = primary.children[n];
                }
                else
                {
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.KouhoSetType();
                    }
                    val2 = val2.GetterC(local);
                    if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
                    if (val2 is Type)
                    {
                        //need later implemented
                        functype.draws.Add(val2 as Type);
                        n++;
                        val2 = primary.children[n];
                        goto head;
                    }
                    else if (val2.type == ObjType.Generic)
                    {
                        var generic = val2 as Generic;
                        n++;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.Block)
                        {
                            local.calls.Add(local.KouhoSetType);
                            var val = val2.exeC(local).GetterC(local);
                            local.calls.RemoveAt(local.calls.Count - 1);
                            if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                            var blk = val as Block;
                            var genericobj = new GenericObj(generic);
                            for (var i = 0; i < blk.rets.Count; i++)
                            {
                                val2 = blk.rets[i];
                                if (val2 is Type)
                                {
                                    genericobj.draws.Add(val2 as Type);
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "ジェネリックスクラスの引数は型でなければいけません"); ;
                            }
                            if (generic.vmap.Count != genericobj.draws.Count) return Obj.Error(ObjType.Error, blk.letter, "ジェネリックスクラスの引数の型の数が合っていません");
                            n++;
                            val2 = primary.children[n];
                            functype.draws.Add(genericobj);
                            goto head;
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "ジェネリックスクラスの引数として[]をお願いします");
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "継承元はクラスでなければならない");
                }
            }
            Word word = null;
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.kouhos = new SortedList<string, Obj>();
                        local.kouhos.Add("new", new Obj(ObjType.None));
                    }
                    var word2 = val2 as Word;
                    n++;
                    if (type.type == ObjType.ArrayType)
                    {
                        if (word2.name == "new") return new ArrayConstructor(type as ArrType) { letter = val2.letter };
                        else return Obj.Error(ObjType.Error, val2.letter, word2.name + "は" + type.letter.name + "の後続にこれません");
                    }
                    else if (type.type == ObjType.ClassObj || type.type == ObjType.ArrayType)
                    {
                        if (word2.name == "new") return new Constructor(type as ClassObj);
                        else return Obj.Error(ObjType.Error, val2.letter, word2.name + "は" + type.letter.name + "の後続にこれません");
                    }
                    else if (type.type == ObjType.ModelObj || type.type == ObjType.GeneObj)
                    {
                        var model = type as ModelObj;
                        var blk0 = model.draw.children[0] as Block;
                        if (blk0.vmapA.ContainsKey(word2.name))
                        {
                            var v = blk0.vmapA[word2.name];
                            if (v.type == ObjType.Variable)
                            {
                                return new SqlString(word2.name, (v as Variable).cls , local);
                            }
                        }
                        var blk1 = model.draw.children[1] as Block;
                        if (blk1.vmapA.ContainsKey(word2.name))
                        {
                            var v = blk1.vmapA[word2.name];
                            if (v.type == ObjType.Variable)
                            {
                                return new SqlString(word2.name, (v as Variable).cls, local);
                            }
                        }
                        if (word2.name == "new") return new Constructor(type as ModelObj) { letter = val2.letter };
                        else return Obj.Error(ObjType.Error, val2.letter, word2.name + "は" + type.letter.name + "の後続にこれません");
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "関数型は.が来れません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には単語が必要です");
            }
            else if (val2.type == ObjType.Dolor)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word2 = val2 as Word;
                    n++;
                    if (local.comments.Count > 0)
                    {
                        var letter = local.comments.Last().ValueAdd(word.name);
                        var variable = new Variable(type) { comment = letter };
                        return type;
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "コメント変数はコメントの中に宣言してください。");
                }
                else return Obj.Error(ObjType.Error, val2.letter, "コメント変数の名前を指定してください。");
            }
            else if (val2.type == ObjType.Word)
            {
                word = val2 as Word;
                n++;
                val2 = primary.children[n];
                var last = local.blocks.Last().obj;
                if (last.obj.type == ObjType.ModelObj || last.obj.type == ObjType.GeneObj)
                {
                    var obj = type;
                head2:
                    if (obj.type == ObjType.ClassObj || obj.type == ObjType.FuncType) return Obj.Error(ObjType.Error, val2.letter, "model,geneの中ではmodelかgeneの型しか宣言できません");
                    else if (obj.type == ObjType.ArrayType)
                    {
                        obj = (obj as ArrType).cls;
                        goto head2;
                    }
                }
            }
            else if (val2.type == ObjType.Bracket)
            {
                var blk = val2.exeC(local) as Block;
                n++;

                for (var i = 0; i < blk.rets.Count; i++)
                {
                    val2 = blk.rets[i];
                    if (val2.type == ObjType.Word)
                    {
                        var variable = local.declareC((val2 as Word).name, local);
                        variable.letter = val2.letter;
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[i], "変数宣言は名前だけです");
                }
                return blk;
            }
            if (val2.type == ObjType.Block)
            {
                var blk = val2.Clone().exeC(local) as Block;
                for (var i = 0; i < blk.rets.Count; i++)
                {
                    if (blk.rets[i].type == ObjType.Word)
                    {
                        var word2 = blk.rets[i] as Word;
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[i], "ジェネリック関数の宣言は名前しか引数をとれません");
                }
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.CallBlock)
                {
                    var func = local.getC2(val2 as CallBlock);
                    n++;
                    if (word != null) local.declareC(word.name, local);
                    return func;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "ジェネリック関数の{|}が指定されていません");
            }
            else if (val2.type == ObjType.CallBlock)
            {
                var func = local.getC2(val2 as CallBlock);
                n++;
                if (word != null)
                {
                    local.declareC(word.name, local);
                    func.letter = word.letter;
                }
                return func;
            }
            else
            {
                if (word == null) return type;
                else
                {
                    var variable = local.declareC(word.name, local);
                    variable.letter = word.letter;
                /*head1:
                    if (val2.type == ObjType.Left)
                    {
                        n++;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.Word)
                        {
                            if (val2.letter == local.letter && local.kouhos == null)
                            {
                                local.KouhoSetFunction();
                            }
                            //create new instantfunc later
                            var func2 = val2.GetterC(local) as Function;
                            n++;
                            val2 = primary.children[n];
                            if (val2.type == ObjType.Block)
                            {
                                n++;
                                val2 = primary.children[n];
                            }
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val2 = primary.children[n];
                            }
                            goto head1;
                        }
                        else
                        {
                            if (val2.type == ObjType.CallBlock)
                            {
                                n++;
                                val2 = primary.children[n];
                                goto head1;
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "進化プログラミングの{|}を指定してください");
                        }
                    }
                    else if (val2.type == ObjType.Right)
                    {
                        n++;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.Word)
                        {
                            if (val2.letter == local.letter && local.kouhos == null)
                            {
                                local.KouhoSetFunction();
                            }
                            var func2 = val2.GetterC(local) as Function;
                            n++;
                            val2 = primary.children[n];
                            //create new instantfunc later
                            if (val2.type == ObjType.Block)
                            {
                                func2.block = val2 as Block;
                                n++;
                                val2 = primary.children[n];
                            }
                            if (val2.type == ObjType.Bracket)
                            {
                                func2.bracket = val2 as Block;
                                n++;
                                val2 = primary.children[n];
                            }
                            goto head1;
                        }
                        else
                        {
                            if (val2.type == ObjType.CallBlock)
                            {
                                n++;
                                val2 = primary.children[n];
                                goto head1;
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "進化プログラミングの{|}を指定してください");
                        }
                    }*/
                    return variable;
                }
            }
        }
    }
    partial class Var
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
    }
    partial class Generic
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Block)
            {
                local.calls.Add(local.KouhoSetType);
                var val = val2.exeC(local).GetterC(local);
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var block = val as Block;
                var geneobj = new GenericObj(this);
                if (this.block.vmap.Count != block.rets.Count) return Obj.Error(ObjType.Error, val2.letter, "ジェネリックスクラスの引数があっていません。");
                for (var i = 0; i < block.rets.Count; i++)
                {
                    if (block.rets[i] is Type)
                    {
                        geneobj.draws.Add(block.rets[i] as Type);
                    }
                    else return Obj.Error(ObjType.Error, block.letters[i], "ジェネリックスクラスは型を引数にとる必要があります。");
                }
                var list = new List<Obj>(this.block.vmap.Values);
            }
            return Obj.Error(ObjType.NG, val2.letter, "ジェネリッククラスは[]を引数にとります");
        }
    }
    partial class Number
    {
        public override Obj GetterC(Local local)
        {
            return new Value(local.Int, new Vari("i32", value.ToString()));
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.kouhos = new SortedList<string, Obj>();
                        local.kouhos.Add("random", new Obj(ObjType.None));
                    }
                    var word = val2 as Word;
                    n++;
                    val2 = primary.children[n];
                    if (word.name == "random")
                    {
                        if (val2.type == ObjType.Bracket)
                        {
                            n++;
                            if (val2.children.Count != 0) return Obj.Error(ObjType.Error, val2.letter, "randomの引数は0個です");
                            return new Number(0) { cls = local.Int };
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "randomの()が指定されていません");
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, word.name + "は適切な後続の単語ではありません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後に単語が来ていません");
            }
            return Obj.Error(ObjType.NG, val2.letter, ".が来ていません");
        }
        public Obj PlusC(String op, Local local, Obj val2)
        {
            if (val2 == null)
            {
                return this;
            }
            if (val2.type == ObjType.Number)
            {
                return new Number(value + (val2 as Number).value) { cls = local.Int };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal((float)value + (val2 as FloatVal).value) { cls = local.Float };
            }
            else if (val2.type == ObjType.StrObj)
            {
                return new StrObj(value + (val2 as StrObj).value) { cls = local.Str };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の+は整数か小数か文字列です");
        }
        public Obj MinusC(String op, Local local, Obj val2)
        {
            if (val2 == null)
            {
                return new Number(-value) { cls = local.Int };
            }
            if (val2.type == ObjType.Number)
            {
                return new Number(value - (val2 as Number).value) { cls = local.Int };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal((float)value - (val2 as FloatVal).value) { cls = local.Float };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の-は整数か小数です");
        }
        public Obj MulC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new Number(value * (val2 as Number).value) { cls = local.Int };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal((float)value * (val2 as FloatVal).value) { cls = local.Float };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の*は整数か小数です");
        }
        public Obj DivC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new Number(value / (val2 as Number).value) { cls = local.Int };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal((float)value / (val2 as FloatVal).value) { cls = local.Float };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の/は整数か小数です");
        }
        public Obj MoreThanC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value > (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value > (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の>は整数か小数です");
        }
        public Obj LessThanC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value < (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value < (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の<は整数か小数です");
        }
        public Obj MoreEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value >= (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value >= (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の>=は整数か小数です");
        }
        public Obj LessEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value <= (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value <= (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の<=は整数か小数です");
        }
        public Obj NotEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value != (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value != (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の!=は整数か小数です");
        }
        public Obj EqualEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value == (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value == (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "整数の==は整数か小数です");
        }

    }
    partial class FloatVal
    {
        public override Obj exeC(Local local)
        {
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, val2.letter, "小数には後続が来れません");
        }
        public Obj PlusC(String op, Local local, Obj val2)
        {
            if (val2 == null) return this;
            if (val2.type == ObjType.Number)
            {
                return new FloatVal(value + (float)(val2 as Number).value) { cls = local.Float };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal(value + (val2 as FloatVal).value) { cls = local.Float };
            }
            else if (val2.type == ObjType.StrObj)
            {
                return new StrObj(value + (val2 as StrObj).value) { cls = local.Str };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の+は整数か小数です");
        }
        public Obj MinusC(String op, Local local, Obj val2)
        {
            if (val2 == null)
            {
                return new FloatVal(-value) { cls = local.Float };
            }
            if (val2.type == ObjType.Number)
            {
                return new FloatVal(value - (float)(val2 as Number).value) { cls = local.Float };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal(value - (val2 as FloatVal).value) { cls = local.Float };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の-は整数か小数です");
        }
        public Obj MulC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new FloatVal(value * (float)(val2 as Number).value) { cls = local.Float };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal(value * (val2 as FloatVal).value) { cls = local.Float };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の*は整数か小数です");
        }
        public Obj DivC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new FloatVal(value / (float)(val2 as Number).value) { cls = local.Float };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new FloatVal(value / (val2 as FloatVal).value) { cls = local.Float };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の/は整数か小数です");
        }
        public Obj MoreThanC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value > (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value > (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の>は整数か小数です");
        }
        public Obj LessThanC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value < (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value < (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の<は整数か小数です");
        }
        public Obj MoreEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value >= (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value >= (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の>=は整数か小数です");
        }
        public Obj LessEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value <= (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value <= (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の<=は整数か小数です");
        }
        public Obj NotEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value != (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value != (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の!=は整数か小数です");
        }
        public Obj EqualEqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Number)
            {
                return new BoolVal(value == (val2 as Number).value) { cls = local.Bool };
            }
            else if (val2.type == ObjType.FloatVal)
            {
                return new BoolVal(value == (val2 as FloatVal).value) { cls = local.Bool };
            }
            return Obj.Error(ObjType.Error, val2.letter, "小数の==は整数か小数です");
        }
    }
    partial class SqlString
    {
        public Dictionary<int, Obj> varis = new Dictionary<int, Obj>();
        public SqlString(string name, Type cls, Local local) : base(ObjType.SqlString)
        {
            value = name;
        }
        public override Obj GetterC(Local local)
        {
            return base.GetterC(local);
        }
        override public Obj exeC(Local local)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return base.exepC(ref n, local, primary);
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return base.PrimaryC(ref n, local, primary, val2);
        }
        public override Obj opeC(string key, Local local, Obj val2)
        {
            if (key == "==" || key == "!=" || key == "<" || key == ">=" || key == "+" || key == "-" || key == "*" || key == "/" || key == "and" || key == "or")
            {
                var k = key;
                if (key == "==") k = "=";
                if (key == "=") value += " " + k + " ";
                else value = "(" + value + " " + k + " ";
                if (val2.type == ObjType.SqlString)
                {
                    SqlString sqs = val2 as SqlString;
                    value += sqs.value + ")";
                    foreach(var v in sqs.varis)
                    {
                        if (!varis.ContainsKey(v.Key))
                        {
                            varis.Add(v.Key, v.Value);
                        }
                    }
                    return this;
                }
                else if (val2.type == ObjType.Value)
                {
                    var value = val2 as Value;
                    if (value.cls == local.Short || value.cls == local.Int || value.cls == local.Float || value.cls == local.Str || value.cls == local.Bool)
                    {
                        var n = LLVM.n++;
                        this.value += "~" + n + ")";
                        varis.Add(n, value);
                        return this;
                    }
                }
            }
            return Obj.Error(ObjType.Error, letter, "文字列の演算子は" + key + "はありません");
        }
        public void Format()
        {
            var map = new Dictionary<int, Obj>();
            var n = 1;
            foreach (var v in varis)
            {
                value.Replace("~" + v.Key, "?" + n);
                map.Add(n, v.Value);
                n++;
            }
            varis = map;
        }
    }
    partial class StrObj
    {
        public override Obj GetterC(Local local)
        {
            var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
            local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
            var v2 = new Vari("i8*", "%v" + LLVM.n++);
            String val = "";
            var n = 1;
            for (var i = 0; i < value.Length; i++)
            {
                if (value[i] >= 256)
                {
                    n = 2;
                    break;
                }
            }
            byte[] utf16le = Encoding.Unicode.GetBytes(value);
            if (n == 1)
            {
                var m = 0;
                foreach (byte b in utf16le)
                {
                    if (m % 2 == 0) val += $"\\{b:X2}";
                    m++;
                }
            }
            else
            {
                foreach (byte b in utf16le)
                {
                    val += $"\\{b:X2}";
                }

            }
            var sv = new StrV("@s" + LLVM.n++, val, value.Length * n);
            local.llvm.strs.Add(sv);
            local.llvm.func.comps.Add(new Gete("[" + (value.Length * n + 1) +" x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
            var v = new Vari("%StringType*", "%v" + LLVM.n++);
            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2,  new Vari("i32", (value.Length * n).ToString()), new Vari("i32", n.ToString())));
            return new Value(local.Str, v);
        }
        public override Obj exeC(Local local)
        {
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, val2.letter, "文字列には後続が来れません");
        }
    }
    partial class BoolVal
    {
        public override Obj exeC(Local local)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, val2.letter, "bool値には後続がこれません");
        }
    }
    partial class VoiVal
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return new Obj(ObjType.Error);
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, val2.letter, "void値には後続がこれません");
        }
    }
    partial class Null
    {
        public override Obj exeC(Local local)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, val2.letter, "nullには後続が来ません");
        }

    }
    partial class ArrayConstructor
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                var val = val2.exeC(local).GetterC(local);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var blk = val as Block;
                if (blk.rets.Count != 1) Obj.Error(ObjType.Error, val2.letter, "配列型のコンストラクタの引数は1です");
                n++;
                if (blk.rets[0].type == ObjType.Number)
                {
                    var value = Value.New(cls, local, letter);
                    if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                    return new Block(ObjType.Array) { rets = new List<Obj> { value }, letters = new Letter[] { letter }, letter = letter };
                }
                else return Obj.Error(ObjType.Error, blk.letters[0], "配列型のコンストラクタの引数は1です");
            }
            var val3 = val2.GetterA(local);
            if (val3.type == ObjType.Wait || val3.type == ObjType.Error || val3.type == ObjType.NG) return val3;
            if (val3.type == ObjType.Number)
            {
                if (val3.type == ObjType.Number)
                {
                    var value = Value.New(cls, local, letter);
                    if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                    return new Block(ObjType.Array) { rets = new List<Obj> { value }, letters = new Letter[] { letter }, letter = letter };
                }
                else return Obj.Error(ObjType.Error, val2.letter, "配列型のコンストラクタの引数は1です");
            }
            return Obj.Error(ObjType.NG, val2.letter, "コンストラクタの引数が適切ではありません");
        }
    }
    partial class Constructor
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                var val = val2.exeC(local).Getter(local);
                var blk = val as Block;
                n++;
                for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                local.comps = new List<Component>();
                if (cls == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
                else if (cls == local.Bool) vari = new Vari("i2", "%v" + LLVM.n++);
                else vari = new Vari(cls.model + "*", "%v" + LLVM.n++);
                local.llvm.func.vari.type = "%FuncType**";
                var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                var load = new Load(fv, local.llvm.func.vari);
                local.llvm.func.comps.Add(load);
                var go_v = new Vari(cls.model + "*", "%v" + LLVM.n++);
                var rn4 = new Vari("%RootNodeType*", "%rn");
                var go_call = new Call(go_v, new Vari(cls.model + "*", cls.drawcall), rn4, fv);
                var varray = (cls.draw.children[0] as Block).vmapA.Values.ToList();
                for (var i = 0; i < varray.Count; i++)
                {
                    if (varray[i].type == ObjType.Variable && blk.rets[i].type == ObjType.Value)
                    {
                        var value = blk.rets[i] as Value;
                        var variable = varray[i] as Variable;
                        if (value.cls == variable.cls) go_call.comps.Add(blk.rets[i].vari);
                        else
                        {
                            if (variable.cls == local.Int)
                            {
                                var v = new Vari("i32", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                go_call.comps.Add(v);
                            }
                            else if (variable.cls == local.Short)
                            {
                                var v = new Vari("i16", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                go_call.comps.Add(v);
                            }
                            else if (variable.cls == local.Bool)
                            {
                                var v = new Vari("i1", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                go_call.comps.Add(v);
                            }
                            else go_call.comps.Add(blk.rets[i].vari);
                        }
                    }
                }
                local.llvm.func.comps.Add(go_call);
                var init_call = new Call(null, new Vari(vari.type, cls.call), rn4, go_v);
                local.llvm.func.comps.Add(init_call);
                return new Value(cls, go_v);
            }
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
            }
            if (val2 != null)
            {
                val2 = val2.GetterC(local);
                if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value)
                {
                    n++;
                    for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                    local.comps = new List<Component>();
                    if (cls == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
                    else if (cls == local.Bool) vari = new Vari("i2", "%v" + LLVM.n++);
                    else vari = new Vari(cls.model + "*", "%v" + LLVM.n++);
                    local.llvm.func.vari.type = "%FuncType**";
                    var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var load = new Load(fv, local.llvm.func.vari);
                    local.llvm.func.comps.Add(load);
                    var go_v = new Vari(cls.model + "*", "%v" + LLVM.n++);
                    var rn4 = new Vari("%RootNodeType*", "%rn");
                    var go_call = new Call(go_v, new Vari(cls.model + "*", cls.drawcall), rn4, fv);
                    var vv = (cls.draw.children[0] as Block).vmapA.Values.ToList()[0] as Variable;
                    if (vv.cls == (val2 as Value).cls) go_call.comps.Add(val2.vari);
                    else
                    {
                        if (vv.cls == local.Int)
                        {
                            var v = new Vari("i32", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                            go_call.comps.Add(v);
                        }
                        else if (vv.cls == local.Short)
                        {
                            var v = new Vari("i16", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                            go_call.comps.Add(v);
                        }
                        else if (vv.cls == local.Short)
                        {
                            var v = new Vari("i1", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                            go_call.comps.Add(v);
                        }
                        else go_call.comps.Add(val2.vari);
                    }
                    go_call.comps.Add(val2.vari);
                    local.llvm.func.comps.Add(go_call);
                    var init_call = new Call(null, new Vari(vari.type, cls.call), rn4, go_v);
                    local.llvm.func.comps.Add(init_call);
                    return new Value(cls, vari);
                }
            }
            return Obj.Error(ObjType.NG, val2.letter, "コンストラクタの引数が適切ではありません");
        }
    }
    partial class Base
    {

        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            var m = 0;
            if (val2.type == ObjType.Block)
            {
                if (val2.children.Count == 1 && val2.children[0].type == ObjType.Number)
                {
                    m = (val2.children[0] as Number).value;
                }
                n++;
                val2 = primary.children[n];
            }
            var cls = extends[m] as ClassObj;
            if (val2.type == ObjType.Bracket)
            {
                local.calls.Add(local.KouhoSet2);
                var val = val2.exeC(local).GetterC(local);
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var blk = val as Block;
                var block1 = cls.draw.children[0] as Block;
                var varr = new List<Obj>(block1.vmapA.Values);
                if (varr.Count != blk.rets.Count) return Obj.Error(ObjType.Error, val2.letter, "baseの引数があっていません");
                for (var i = 0; i < varr.Count; i++)
                {
                    varr[i].opesC["="]("=", local, blk.rets[i]);
                }
                return new VoiVal();
            }
            if (val2.letter == local.letter && local.kouhos == null)
            {
                local.KouhoSet2();
            }
            val2 = val2.GetterC(local);
            if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value)
            {
                var block1 = cls.draw.children[0] as Block;
                var varr = new List<Obj>(block1.vmapA.Values);
                if (varr.Count != 1) return Obj.Error(ObjType.Error, val2.letter, "base元のクラスの引数は1個ではありません");
                for (var i = 0; i < varr.Count; i++)
                {
                    varr[i].opesC["="]("=", local, val2);
                }
                n++;
                return new VoiVal();
            }
            return Obj.Error(ObjType.Error, val2.letter, "baseの引数が適切に設定されていません");
        }
    }
    partial class Variable
    {
        public bool clone;
        public Vari strvari;
        public Variable(Type cls, Vari vari) : base(ObjType.Variable)
        {
            this.cls = cls;
            this.vari = vari;
            clone = true;
        }
        public override Obj GetterC(Local local)
        {
            if (this.vari.type == "[n]")
            {
                this.vari.type = "i8**";
                var v = new Vari("i8*", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(v, this.vari));
                if (cls.type == ObjType.Var) cls = (cls as Var).cls;
                if (cls == local.Int)
                {
                    var v2 = new Vari("i32", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new PtrToInt(v2, v));
                    return new Value(cls, v2);
                }
                else if (cls == local.Bool)
                {
                    var v2 = new Vari("i1", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new PtrToInt(v2, v));
                    return new Value(cls, v2);
                }
                else if (cls.type == ObjType.FuncType)
                {
                    var functype = cls as FuncType;
                    var func = new Function(functype.cls, functype.draws, v);
                    return func;
                }
                else if (cls.type == ObjType.ArrayType)
                {
                    return new Value(cls, v);
                }
                else
                {
                    this.vari = new Vari(cls.model + "*", "%v" + LLVM.n++);
                    return new Value(cls, v);
                }
            }
            else if (this.vari.type == "[s]")
            {
                this.vari.type = "%HashType*";
                var varig = new Vari("%GetHashType", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(varig, new Vari("%GetHashType*", "@GetHash")));
                var v = new Vari(cls.model + "*", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Call(v, varig, this.vari, strvari));
                if (cls == local.Int)
                {
                    var v2 = new Vari("i32", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new PtrToInt(v2, v));
                    return new Value(cls, v2);
                }
                else if (cls == local.Bool)
                {
                    var v2 = new Vari("i1", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new PtrToInt(v2, v));
                    return new Value(cls, v2);
                }
                else if (cls.type == ObjType.FuncType)
                {
                    var functype = cls as FuncType;
                    var func = new Function(functype.cls, functype.draws, v);
                    return func;
                }
                else if (cls.type == ObjType.ArrayType)
                {
                    return new Value(cls, v);
                }
                else
                {
                    return new Value(cls, v);
                }
            }
            else if (this.vari.type == "s[n]")
            {
                this.vari.type = "%StringType*";
                var varig = new Vari("%GetCharType", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(varig, new Vari("%GetCharType*", "@GetChar")));
                var v = new Vari("i16", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Call(v, varig, this.vari, strvari));
                return new Value(cls, v);
            }
            for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
            local.comps = new List<Component>();
            if (cls.type == ObjType.Var) cls = (cls as Var).cls;
            Vari vari = null;
            if (cls == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
            else if (cls == local.Short) vari = new Vari("i16", "%v" + LLVM.n++);
            else if (cls == local.Bool) vari = new Vari("i1", "%v" + LLVM.n++);
            else if (cls.type == ObjType.FuncType)
            {
                var functype = cls as FuncType;
                vari = new Vari("%FuncType*", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(vari, this.vari));
                var func = new Function(functype.cls, functype.draws, vari);
                return func;
            }
            else if (cls.type == ObjType.ArrayType)
            {
                var arrtype = cls as ArrType;
                vari = new Vari("%HashType*", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(vari, this.vari));
                return new Value(cls, vari);
            }
            else vari = new Vari(cls.model + "*", "%v" + LLVM.n++);
            local.llvm.func.comps.Add(new Load(vari, this.vari));
            return new Value(cls, vari);
        }
        public override Obj SelfC(Local local)
        {
            if (clone) return this;
            for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
            local.comps = new List<Component>();
            if (cls.type == ObjType.Var) cls = (cls as Var).cls;
            if (cls == local.Int) local.llvm.func.vari.type = "i32*";
            else if (cls == local.Short) local.llvm.func.vari.type = "i16*";
            else if (cls == local.Bool) local.llvm.func.vari.type = "i1*";
            else if (cls.type == ObjType.FuncType) local.llvm.func.vari.type = "%FuncType**";
            else if (cls.type == ObjType.ArrayType) local.llvm.func.vari.type = "%HashType**";
            else local.llvm.func.vari.type = cls.model + "**";
            var variable = Clone();
            variable.vari = local.llvm.func.vari;
            return variable;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Value.New(cls, local, letter).PrimaryC(ref n, local, primary, val2);
        }
        public override void Dec(Local local)
        {
        }
        public Obj EqualC(String op, Local local, Obj val2)
        {
            if (this.vari.type == "[n]")
            {
                this.vari.type = "i8**";
                if (cls == local.Int || cls == local.Short)
                {
                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new IntToPtr(v2, val2.vari));
                    local.llvm.func.comps.Add(new Store(this.vari, v2));
                    return new Value(cls, v2);
                }
                else if (cls == local.Bool)
                {
                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new IntToPtr(v2, val2.vari));
                    local.llvm.func.comps.Add(new Store(this.vari, v2));
                    return new Value(cls, v2);
                }
                else if (cls.type == ObjType.FuncType)
                {
                    local.llvm.func.comps.Add(new Store(this.vari, val2.vari));
                    var functype = cls as FuncType;
                    var func = new Function(functype.cls, functype.draws, this.vari);
                    return func;
                }
                else if (cls.type == ObjType.ArrayType)
                {
                    local.llvm.func.comps.Add(new Store(this.vari, val2.vari));
                    var arrtype = cls as ArrType;
                    return new Value(cls, this.vari);
                }
                else
                {
                    local.llvm.func.comps.Add(new Store(this.vari, val2.vari));
                    return new Value(cls, this.vari);
                }
            }
            else if (this.vari.type == "[s]")
            {
                this.vari.type = "%HashType*";
                var varise = new Vari("%SetHashType", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(varise, new Vari("%SetHashType*", "@SetHash")));
                if (cls == local.Int || cls == local.Short)
                {
                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new IntToPtr(v2, val2.vari));
                    local.llvm.func.comps.Add(new Call(null, varise, this.vari, strvari, v2));
                    return new Value(cls, v2);
                }
                else if (cls == local.Bool)
                {
                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new IntToPtr(v2, val2.vari));
                    local.llvm.func.comps.Add(new Call(null, varise, this.vari, strvari, v2));
                    return new Value(cls, v2);
                }
                else if (cls.type == ObjType.FuncType)
                {
                    local.llvm.func.comps.Add(new Store(this.vari, val2.vari));
                    var functype = cls as FuncType;
                    var func = new Function(functype.cls, functype.draws, this.vari);
                    local.llvm.func.comps.Add(new Call(null, varise, this.vari, strvari, val2.vari));
                    return func;
                }
                else if (cls.type == ObjType.ArrayType)
                {
                    local.llvm.func.comps.Add(new Call(null, varise, this.vari, strvari, val2.vari));
                    var arrtype = cls as ArrType;
                    return new Value(cls, this.vari);
                }
                else
                {
                    local.llvm.func.comps.Add(new Call(null, varise, this.vari, strvari, val2.vari));
                    return new Value(cls, this.vari);
                }
            }
            else if (this.vari.type == "s[n]")
            {
                return Obj.Error(ObjType.Error, letter, "文字列に代入はできません");
            }
            if (val2.type == ObjType.Value)
            {
                var value = val2 as Value;
                var va = Bitcast.Cast(local, cls, value.cls, val2.vari);
                local.llvm.func.comps.Add(new Store(this.vari, va));
            }
            else return Obj.Error(ObjType.Error, val2.letter, "変数への代入が不正です");
            var variable = Clone();
            variable.vari = this.vari;
            return variable;
        }
        public Obj InC(String op, Local local, Obj val2)
        {
            if (cls.type == ObjType.Var)
            {
                var varobj = cls as Var;
                if (varobj.cls == null) return Obj.Error(ObjType.Wait, letter, "varの型が判明しません");
            }
        head:
            if (val2.type == ObjType.Bracket)
            {
                local.calls.Add(local.KouhoSet2);
                var val = val2.exeC(local).GetterC(local);
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var block = val as Block;
                if (block.rets.Count == 1)
                {
                    val2 = block.rets[0];
                    goto head;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "()の中の数が間違っています。");
            }
            val2 = val2.GetterC(local);
            if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            if (val2.type == ObjType.Block)
            {
                var it = new Iterator(-1);
                var val = val2;
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                it.value = val as Block;
                value = it;
                return it;
            }
            else return Obj.Error(ObjType.Error, val2.letter, "Iteratorの代入が不正です");
        }

    }
    partial class Function
    {
        public List<Type> draws;
        public Function(Type ret, List<Type> draws, Vari vari): base(ObjType.Function)
        {
            this.ret = ret;
            this.draws = draws;
            this.vari = vari;
        }
        public override Obj GetterC(Local local)
        {
            for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
            local.comps = new List<Component>();
            var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
            if (draws == null)
            {
                local.llvm.func.vari.type = "%FuncType**";
                var load = new Load(fv, local.llvm.func.vari);
                local.llvm.func.comps.Add(load);
            }
            else
            {
                var load = new Load(fv, this.vari);
                local.llvm.func.comps.Add(load);
            }
            this.vari = fv;
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (bracket != null) throw new Exception();
            if (val2.type == ObjType.Bracket)
            {
                var val = val2.exeC(local).GetterC(local);
                Vari vari;
                if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                if (ret == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
                else if (ret == local.Bool) vari = new Vari("i2", "%v" + LLVM.n++);
                else if (ret.type == ObjType.FuncType)
                {
                    var functype = ret as FuncType;
                    vari = new Vari("%FuncType*", "%v" + LLVM.n++);
                }
                else vari = new Vari(ret.model + "*", "%v" + LLVM.n++);
                var blk = val as Block;
                n++;
                if (draws == null)
                {
                    var go_v = new Vari(model + "*", "%v" + LLVM.n++);
                    var go_call = new Call(go_v, new Vari(model + "*", drawcall), this.vari);
                    var varray = (draw.children[0] as Block).vmapA.Values.ToList();
                    for (var i = 0; i < varray.Count; i++)
                    {
                        if (varray[i].type == ObjType.Variable && blk.rets[i].type == ObjType.Value)
                        {
                            var value = blk.rets[i] as Value;
                            var variable = varray[i] as Variable;
                            if (value.cls == variable.cls) go_call.comps.Add(blk.rets[i].vari);
                            else
                            {
                                if (variable.cls == local.Int)
                                {
                                    var v = new Vari("i32", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                    go_call.comps.Add(v);
                                }
                                else if (variable.cls == local.Short)
                                {
                                    var v = new Vari("i16", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                    go_call.comps.Add(v);
                                }
                                else if (variable.cls == local.Bool)
                                {
                                    var v = new Vari("i1", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                    go_call.comps.Add(v);
                                }
                                else go_call.comps.Add(blk.rets[i].vari);
                            }
                        }
                    }
                    var rn4 = new Vari("%RootNodeType*", "%rn");
                    local.llvm.func.comps.Add(go_call);
                    var init_call = new Call(vari, new Vari(vari.type, call), rn4, go_v);
                    local.llvm.func.comps.Add(init_call);
                }
                else
                {
                    var dv = new Vari("i8**", "%v" + LLVM.n++);
                    var gete = new Gete("%FuncType", dv, this.vari, new Vari("i32", "0"), new Vari("i32", "1"));
                    local.llvm.func.comps.Add(gete);
                    var dv2 = new Vari("i8*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(dv2, dv));
                    var go_v = new Vari("i8*", "%v" + LLVM.n++);
                    var go_call = new Call(go_v, dv2, this.vari);
                    for (var i = 0; i < blk.rets.Count; i++)
                    {
                        if (blk.rets[i].type == ObjType.Value)
                        {
                            var value = blk.rets[i] as Value;
                            if (value.cls == draws[i]) go_call.comps.Add(blk.rets[i].vari);
                            else
                            {
                                if (draws[i] == local.Int)
                                {
                                    var v = new Vari("i32", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                    go_call.comps.Add(v);
                                }
                                else if (draws[i] == local.Short)
                                {
                                    var v = new Vari("i16", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                    go_call.comps.Add(v);
                                }
                                else if (draws[i] == local.Bool)
                                {
                                    var v = new Vari("i1", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, blk.rets[i].vari));
                                    go_call.comps.Add(v);
                                }
                                else go_call.comps.Add(blk.rets[i].vari);
                            }
                        }
                    }
                    local.llvm.func.comps.Add(go_call);
                    var cv = new Vari("i8**", "%v" + LLVM.n++);
                    var cgete = new Gete("%FuncType", cv, this.vari, new Vari("i32", "0"), new Vari("i32", "2"));
                    local.llvm.func.comps.Add(cgete);
                    var cv2 = new Vari("i8*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(cv2, cv));
                    var rn4 = new Vari("%RootNodeType*", "%rn");
                    var init_call = new Call(vari, cv2, rn4, go_v);
                    local.llvm.func.comps.Add(init_call);
                }
                return new Value(ret, vari);
            }
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
            }
            if (val2 != null)
            {
                val2 = val2.GetterC(local);
                if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value)
                {
                    n++;
                    Vari vari;
                    if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                    if (ret == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
                    else if (ret == local.Bool) vari = new Vari("i2", "%v" + LLVM.n++);
                    else if (ret.type == ObjType.FuncType)
                    {
                        var functype = ret as FuncType;
                        vari = new Vari("%FuncType*", "%v" + LLVM.n++);
                    }
                    else vari = new Vari(ret.model + "*", "%v" + LLVM.n++);
                    if (draws == null)
                    {
                        var go_v = new Vari(model + "*", "%v" + LLVM.n++);
                        var go_call = new Call(go_v, new Vari(model + "*", drawcall), this.vari);
                        var vv = (draw.children[0] as Block).vmapA.Values.ToList()[0] as Variable;
                        if (vv.cls == (val2 as Value).cls) go_call.comps.Add(val2.vari);
                        else
                        {
                            if (vv.cls == local.Int)
                            {
                                var v = new Vari("i32", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                go_call.comps.Add(v);
                            }
                            else if (vv.cls == local.Short)
                            {
                                var v = new Vari("i16", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                go_call.comps.Add(v);
                            }
                            else if (vv.cls == local.Bool)
                            {
                                var v = new Vari("i1", "%v" + LLVM.n++);
                                local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                go_call.comps.Add(v);
                            }
                            else go_call.comps.Add(val2.vari);
                        }
                        local.llvm.func.comps.Add(go_call);
                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var init_call = new Call(vari, new Vari(vari.type, call), rn4, go_v);
                        local.llvm.func.comps.Add(init_call);
                    }
                    else
                    {
                        var dv = new Vari("i8**", "%v" + LLVM.n++);
                        var gete = new Gete("%FuncType", dv, this.vari, new Vari("i32", "0"), new Vari("i32", "1"));
                        local.llvm.func.comps.Add(gete);
                        var dv2 = new Vari("i8*", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(dv2, dv));
                        var go_v = new Vari("i8*", "%v" + LLVM.n++);
                        var go_call = new Call(go_v, dv2, this.vari);
                        if (val2.type == ObjType.Value)
                        {
                            var value = val2 as Value;
                            if (value.cls == draws[0]) go_call.comps.Add(val2.vari);
                            else
                            {
                                if (draws[0] == local.Int)
                                {
                                    var v = new Vari("i32", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    go_call.comps.Add(v);
                                }
                                else if (draws[0] == local.Short)
                                {
                                    var v = new Vari("i16", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    go_call.comps.Add(v);
                                }
                                else if (draws[0] == local.Bool)
                                {
                                    var v = new Vari("i1", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    go_call.comps.Add(v);
                                }
                                else go_call.comps.Add(val2.vari);
                            }
                        }
                        else return Obj.Error(ObjType.NG, val2.letter, "関数の引数が適切ではありません");
                        local.llvm.func.comps.Add(go_call);
                        var cv = new Vari("i8**", "%v" + LLVM.n++);
                        var cgete = new Gete("%FuncType", cv, this.vari, new Vari("i32", "0"), new Vari("i32", "2"));
                        local.llvm.func.comps.Add(cgete);
                        var cv2 = new Vari("i8*", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(cv2, cv));
                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var init_call = new Call(vari, cv2, rn4, go_v);
                        local.llvm.func.comps.Add(init_call);
                    }
                    if (ret.type == ObjType.FuncType)
                    {
                        var functype = ret as FuncType;
                        return new Function(functype.cls, functype.draws, vari);
                    }
                    return new Value(ret, vari);
                }
            }
            return Obj.Error(ObjType.NG, val2.letter, "()のような関数に適切な引数が来ていません");
        }
        public override string model
        {
            get { return "%" + this.letter.name + "FuncType" + identity; }
        }
        public override string call
        {
            get { return "@" + this.letter.name + identity; }
        }
        public override string drawcall
        {
            get { return "@" + this.letter.name + "Draw" + identity; }
        }
        public int identity;
        public override void Dec(Local local)
        {
            if (identity == 0) identity = LLVM.n++;
            ifv = new Dictionary<string, IfValue>();
            String type;
            if (ret.type == ObjType.Var) ret = (ret as Var).cls;
            if (ret == local.Int) type = "i32";
            else if (ret == local.Bool) type = "i1";
            else type = ret.model + "*";
            this.n = Obj.cn++;
            this.vari = new Vari("void", call);
            var rn4 = new Vari("%RootNodeType*", "%rn");
            var obj = new Vari(this.model + "*", "%obj");
            var func = new Func(local.llvm, this.vari, rn4, obj);
            local.llvm.comps.Add(func);
            local.llvm.funcs.Add(func);
            var thgcptr4 = new Vari("%ThreadGCType**", "%thgcptr");
            func.comps.Add(new Gete("%RootNodeType", thgcptr4, rn4, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            func.comps.Add(new Load(thgc4, thgcptr4));

            var objptr = new Vari(this.model + "**", "%objptr");
            var alloca = new Alloca(objptr);
            func.comps.Add(alloca);
            var objstore = new Store(objptr, obj);
            func.comps.Add(objstore);
            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            func.comps.Add(srload);
            var srcall = new Call(null, srv, rn4, objptr);
            func.comps.Add(srcall);


            var rn5 = new Vari("%RootNodeType*", "%rn");
            var blk = new Vari("i8*", "%fptr1");
            var funcdraw = new Func(local.llvm, new Vari(this.model + "*", drawcall), rn5, blk);
            local.llvm.comps.Add(funcdraw);

            var typedec = new TypeDec(this.model);
            local.llvm.types.Add(typedec);
            typedec.comps.Add(new TypeVal("i8*", "blk"));

            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var checkname = "@" + this.letter.name + "Check" + identity;
            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            var countv = new Vari("i32", "0");
            var strv = new StrV("@" + this.letter.name + identity, "@" + this.letter.name, this.letter.name.Length * 1);
            local.llvm.strs.Add(strv);
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var ac_call = new Call(null, ac_val, thgc3, strv, countv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var thgcptr5 = new Vari("%ThreadGCType**", "%thgcptr");
            funcdraw.comps.Add(new Gete("%RootNodeType", thgcptr5, rn5, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc5 = new Vari("%ThreadGCType*", "%thgc");
            funcdraw.comps.Add(new Load(thgc5, thgcptr5));
            var go_val = new Vari("%CopyObjectType", "%gcobject");
            var go_load = new Load(go_val, new Vari("%CopyObjectType*", "@CloneObject"));
            funcdraw.comps.Add(go_load);
            var v3 = new Vari("i8**", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", v3, blk, new Vari("i32", "0"), new Vari("i32", "3"));
            funcdraw.comps.Add(gete);
            var v4 = new Vari("i8*", "%v" + LLVM.n++);
            funcdraw.comps.Add(new Load(v4, v3));
            var go_v = new Vari("i8*", "%obj");
            var go_call = new Call(go_v, go_val, thgc5, v4);
            funcdraw.comps.Add(go_call);
            var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
            var go_cast = new Bitcast(go_c, go_v);
            funcdraw.comps.Add(go_cast);

            var rn6 = new Vari("%RootNodeType*", "%rn");
            var block = new Vari("i8*", "%block");
            var funcptr = new Vari("i8*", "%fptr");
            var funcptr2 = new Vari("i8*", "%fptr2");
            var decname = "@" + this.letter.name + "dec" + identity;
            var funcdec = new Func(local.llvm, new Vari("%FuncType*", decname), rn6, block, funcptr, funcptr2);
            local.llvm.comps.Add(funcdec);
            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
            funcdec.comps.Add(new Load(thgc6, new Vari("%ThreadGCType**", "@thgcp")));

            var gmvari = new Vari("%GC_mallocType", "%gm");
            var gmload = new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc"));
            funcdec.comps.Add(gmload);
            var tmp = new Vari("i32", 28.ToString());
            go_call = new Call(go_v, gmvari, thgc6, tmp);
            funcdec.comps.Add(go_call);
            var go_c3 = new Vari("%FuncType*", "%v" + LLVM.n++);
            go_cast = new Bitcast(go_c3, go_v);
            funcdec.comps.Add(go_cast);

            var vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc, go_c3, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);
            var vc2 = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc2, go_c3, new Vari("i32", "0"), new Vari("i32", "1"));
            funcdec.comps.Add(gete);
            store = new Store(vc2, funcptr);
            funcdec.comps.Add(store);
            var vc3 = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc3, go_c3, new Vari("i32", "0"), new Vari("i32", "2"));
            funcdec.comps.Add(gete);
            store = new Store(vc3, funcptr2);
            funcdec.comps.Add(store);

            var objptr2 = new Vari("%FuncType**", "%objptr");
            var alloca2 = new Alloca(objptr2);
            funcdec.comps.Add(alloca2);
            var objstore2 = new Store(objptr2, go_c3);
            funcdec.comps.Add(objstore2);

            srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            funcdec.comps.Add(srload);
            srcall = new Call(null, srv, rn6, objptr2);
            funcdec.comps.Add(srcall);


            go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
            tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            funcdec.comps.Add(new Add(tmv, tmi, tmp));
            go_call = new Call(go_v, gmvari, thgc6, tmv);
            funcdec.comps.Add(go_call);
            var go_c2 = new Vari(this.model + "*", "%v" + LLVM.n++);
            go_cast = new Bitcast(go_c2, go_v);
            funcdec.comps.Add(go_cast);

            vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete(this.model, vc, go_c2, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);

            var objptr3 = new Vari(this.model + "**", "%v" + LLVM.n++);
            var alloca3 = new Alloca(objptr3);
            funcdec.comps.Add(alloca3);
            var objstore3 = new Store(objptr3, go_c2);
            funcdec.comps.Add(objstore3);

            srcall = new Call(null, srv, thgc6, objptr3);
            funcdec.comps.Add(srcall);

            var vc4 = new Vari(this.model + "**", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc4, go_c3, new Vari("i32", "0"), new Vari("i32", "3"));
            funcdec.comps.Add(gete);
            var parentstore = new Store(vc4, go_c2);
            funcdec.comps.Add(parentstore);

            int count = 8;
            var blk0 = draw.children[0] as Block;
            int order = 1;
            var blk2 = draw.children[1] as Block;
            var decs = new List<Obj>();
            foreach (var kv in blk0.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        var vdraw = new Vari("i32", "%v" + LLVM.n++);
                        funcdraw.draws.Add(vdraw);
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;

                        var varii = new Vari("i32*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);
                    }
                    else if (variable.cls == local.Bool)
                    {
                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                        funcdraw.draws.Add(new Vari("i32", "%v" + LLVM.n++));
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;

                        var varii = new Vari("i1*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        funcdraw.draws.Add(vdraw);
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        var varii = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    funcdraw.draws.Add(new Vari(f.model + "*", "%v" + LLVM.n++));
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                    funcdraw.draws.Add(new Vari("%FuncType*", "%v" + LLVM.n++));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);
                }
            }
            foreach (var kv in blk2.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    decs.Add(f);
                    f.order = order++;
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);
                }
            }
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            local.llvm.funcs.Add(funcdec);
            this.bas = objptr2;
            local.blocks.Add(blk0);
            blk0.exeC(local);
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            local.blocks.Add(blk2);

            funcdraw.comps.Add(new Ret(go_c));
            countv.name = count.ToString();
            funccheck.comps.Add(new Ret(new Vari("void", null)));
            var vv = new Vari("%FuncType*", "%v" + LLVM.n++);
            funcdec.comps.Add(new Load(vv, objptr2));

            var rnpv = new Vari("i32*", "%ptr");
            funcdec.comps.Add(new Gete("%RootNodeType", rnpv, rn6, new Vari("i32", "0"), new Vari("i32", "1")));
            var rnpv12 = new Vari("i32", "%v");
            var rnp12load = new Load(rnpv12, rnpv);
            funcdec.comps.Add(rnp12load);
            var rnpv2 = new Vari("i32", "%dec");
            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            funcdec.comps.Add(rnpsub);
            funcdec.comps.Add(new Store(rnpv, rnpv2));
            funcdec.comps.Add(new Ret(go_c3));

            this.bas = objptr;
            local.labs.Add(new Lab("entry"));
            blk2.exeC(local);
            local.labs.RemoveAt(local.labs.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);

            rnpv = new Vari("i32*", "%ptr");
            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
            rnpv12 = new Vari("i32", "%v");
            rnp12load = new Load(rnpv12, rnpv);
            local.llvm.func.comps.Add(rnp12load);
            rnpv2 = new Vari("i32", "%dec");
            rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            local.llvm.func.comps.Add(rnpsub);
            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
            var valobj = blk2.rets.Last();
            if (valobj.type == ObjType.Value)
            {
                var value = valobj as Value;
                var va = Bitcast.Cast(local, ret, value.cls, valobj.vari);
                local.llvm.func.comps.Add(new Ret(va));
            }
            else throw new Exception("");
            local.llvm.func.comps.Add(new Ret(blk2.rets.Last().vari));
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
        }
    }
    partial class IfBlock
    {
        public override string model {
            get { return iflabel.model; }
        }
    }
    partial class If
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            var values = new Dictionary<String, IfValue>();
            if (val2.type == ObjType.CallBlock)
            {
                var blk0 = val2.children[0] as Block;
                var blk2 = val2.children[1] as Block;
                n++;

                val2 = primary.children[n];
                var ifb = calls[0];
                if (ret.cls == null) return Obj.Error(ObjType.Wait, letter, "varの型が判明しません");
                var m = 1;
                this.n = Obj.cn++;
                this.model = "%" + this.letter.name + "IfType";
                var typedec = new TypeDec(this.model);
                local.llvm.types.Add(typedec);
                var checkname = "@" + this.letter.name + "IfCheck" + LLVM.n++;
                var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                var i8p = new Vari("i8*", "%self");
                var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
                local.llvm.comps.Add(funccheck);
                typedec.comps.Add(new TypeVal("i8*", "blk"));
                var vari = new Vari("i8*", "%v" + LLVM.n++);
                var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
                funccheck.comps.Add(gete);
                var co_val = new Vari("%CopyObjectType", "%co");
                var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
                funccheck.comps.Add(co_load);
                var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                var co_lod = new Load(vari2, vari);
                funccheck.comps.Add(co_lod);
                var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                var co_call = new Call(co_cval, co_val, thgc2, vari);
                funccheck.comps.Add(co_call);
                var store = new Store(vari, co_cval);
                funccheck.comps.Add(store);

                var thgc3 = new Vari("%ThreadGCType*", "%thgc");
                var ac_val = new Vari("%GC_AddClassType", "%addclass");
                var countv = new Vari("i32", "");
                var name = "if" + LLVM.n++;
                var strv = new StrV("@" + name, name, name.Length * 1);
                local.llvm.strs.Add(strv);
                var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
                var tnp = new Vari("i32", "%tnp" + LLVM.n++);
                local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                var tv = new Vari("i32", "%cv" + LLVM.n++);
                local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                var ac_call = new Call(null, ac_val, thgc3, strv, countv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                local.llvm.main.comps.Add(ac_call);

                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                var go_val = new Vari("%GC_mallocType", "%gcobject" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(go_val, new Vari("%GC_mallocType*", "@GC_malloc")));
                var go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
                var tmp = new Vari("i32", this.n.ToString());
                var tmi = new Vari("i32", "%tmi" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                var tmv = new Vari("i32", "%tmv" + LLVM.n++);
                local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
                var go_call = new Call(go_v, go_val, thgc4, tmv);
                local.llvm.func.comps.Add(go_call);
                var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
                var go_cast = new Bitcast(go_c, go_v);
                local.llvm.func.comps.Add(go_cast);

                Vari objptr3;

                var v0 = new Vari("i8**", "%v" + LLVM.n++);
                gete = new Gete(this.model, v0, go_c, new Vari("i32", "0"), new Vari("i32", "0"));
                local.llvm.func.comps.Add(gete);
                var gj = new Vari("%GCObjectPtr", "%gj" + LLVM.n++);
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Load(gj, objp));
                }
                else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(gj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                else local.llvm.func.comps.Add(new Load(gj, local.blocks.Last().obj.obj.bas));
                local.llvm.func.comps.Add(new Store(v0, gj));



                var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    objptr3 = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objptr3, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Store(objptr3, go_c));
                }
                else {
                    objptr3 = new Vari(this.model + "**", "%objptr" + LLVM.n++);
                    var alloca = new Alloca(objptr3);
                    local.llvm.func.comps.Add(alloca);
                    var objstore = new Store(objptr3, go_c);
                    local.llvm.func.comps.Add(objstore);
                    this.bas = objptr3;
                    var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
                    local.llvm.func.comps.Add(srload);
                    var srcall = new Call(null, srv, thgc4, this.bas);
                    local.llvm.func.comps.Add(srcall);
                }
                int count = 8;
                int order = 1;
                var decs = new List<Obj>();
                foreach (var kv in blk0.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal("%" + variable.cls.letter.name + "Type*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                local.blok = go_c;
                local.typedec = typedec;
                local.funccheck = funccheck;
                local.funcdec = local.llvm.func;
                local.i8p = i8p;
                local.objptr3 = objptr3;
                local.co_val = co_val;
                local.countv = countv;
                local.odr = order;
                local.count = count;
                for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                decs = new List<Obj>();
                countv.name = count.ToString();
                funccheck.comps.Add(new Ret(new Vari("void", null)));
                local.blocks.Add(blk0);
                if (local.llvm.func.async)
                {
                    //Br checks in if,elif,else
                }
                blk0.exeC(local);

                var lab1 = new Lab("then" + LLVM.n++);
                var lab2 = new Lab("else" + LLVM.n++);
                var end = new Lab("end");
                if (calls.Count == 1) lab2 = end;
                this.entry = lab1;
                this.end = end;
                var br = new Br(blk0.rets.Last().vari, lab1, lab2);
                local.llvm.func.comps.Add(br);
                local.llvm.func.comps.Add(lab1);
                local.blocks.Add(blk2);
                local.labs.Add(lab1);
                blk2.exeC(local);
                local.labs.RemoveAt(local.labs.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);

                var rn4 = new Vari("%RootNodeType*", "%rn");

                if (!values.ContainsKey(lab1.name))
                {
                    if (blk2.rets.Last().type == ObjType.Value)
                    {
                        var value = blk2.rets.Last() as Value;
                        var va = Bitcast.Cast(local, ret, value.cls, blk2.vari);
                        values[lab1.name] = new IfValue(lab1, va);
                        local.llvm.func.comps.Add(new Br(null, end));
                    }
                    else if (blk2.rets.Last().type == ObjType.VoiVal)
                    {
                        local.llvm.func.comps.Add(new Br(null, end));
                    }
                    else return Obj.Error(ObjType.Error, letter, "ifの値がありません");
                }
                if (calls.Count != 1)
                {
                    local.llvm.func.comps.Add(lab2);
                }
            head:
                var elif = val2;
                if (letter == local.letter && local.kouhos == null)
                {
                    local.calls.Last()();
                }
                if (val2.type == ObjType.Elif)
                {
                    if (local.llvm.func.async)
                    {
                        var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                        var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(objv, objp));
                        var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                        var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(blockv, blockptr));
                        local.llvm.func.comps.Add(new Store(objp, blockv));
                    }
                    else
                    {
                        var rnpv = new Vari("i32*", "%ptr");
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v");
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                    }
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.CallBlock)
                    {
                        var blk10 = val2.children[0] as Block;
                        var blk12 = val2.children[1] as Block;
                        elif.model = "%" + elif.letter.name + "ElifType";
                        var typedec1 = new TypeDec(elif.model);
                        local.llvm.types.Add(typedec1);
                        typedec1.comps.Add(new TypeVal("i8*", "blk"));

                        var i8p1 = new Vari("i8*", "%self");
                        checkname = "@" + elif.letter.name + "ElifCheck" + LLVM.n++;
                        var thgc21 = new Vari("%ThreadGCType*", "%thgc");
                        var funccheck2 = new Func(local.llvm, new Vari("void", checkname), thgc21, i8p1);
                        var vari1 = new Vari("i8*", "%v" + LLVM.n++);
                        var gete1 = new Gete(elif.model, vari1, i8p1, new Vari("i32", "0"), new Vari("i32", "0"));
                        funccheck2.comps.Add(gete1);
                        var co_val1 = new Vari("%CopyObjectType", "%co");
                        var co_load1 = new Load(co_val1, new Vari("%CopyObjectType*", "@CopyObject"));
                        funccheck2.comps.Add(co_load1);
                        var vari12 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_lod1 = new Load(vari12, vari1);
                        funccheck2.comps.Add(co_lod1);
                        var co_cval1 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_call1 = new Call(co_cval1, co_val1, thgc21, vari1);
                        funccheck2.comps.Add(co_call1);
                        var store1 = new Store(vari1, co_cval1);
                        funccheck2.comps.Add(store1);
                        local.llvm.comps.Add(funccheck2);

                        var ac_val1 = new Vari("%GC_AddClassType", "%addclass");
                        var countv1 = new Vari("i32", "");
                        var name1 = "elif" + LLVM.n++;
                        var strv1 = new StrV("@" + name1, name1, name1.Length * 1);
                        local.llvm.strs.Add(strv1);
                        typ = new Vari("i32", (elif.n = local.llvm.cn++).ToString());
                        tnp = new Vari("i32", "%tnp" + LLVM.n++);
                        local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                        tv = new Vari("i32", "%cv" + LLVM.n++);
                        local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                        var ac_call1 = new Call(null, ac_val1, thgc3, countv1, strv1, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                        local.llvm.main.comps.Add(ac_call1);

                        var go_v1 = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
                        tmp = new Vari("i32", elif.n.ToString());
                        tmi = new Vari("i32", "%tmi" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                        tmv = new Vari("i32", "%tmv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
                        var go_call1 = new Call(go_v1, go_val, thgc3, tmv);
                        local.llvm.func.comps.Add(go_call1);
                        var go_c1 = new Vari(elif.model + "*", "%v" + LLVM.n++);
                        var go_cast1 = new Bitcast(go_c1, go_v1);
                        local.llvm.func.comps.Add(go_cast1);

                        var objptr31 = new Vari(elif.model + "**", "%objptr" + LLVM.n++);

                        var v01 = new Vari("i8**", "%v" + LLVM.n++);
                        gete = new Gete(elif.model, v01, go_c1, new Vari("i32", "0"), new Vari("i32", "0"));
                        local.llvm.func.comps.Add(gete);

                        gj = new Vari("%GCObjectPtr", "%gj" + LLVM.n++);
                        if (local.llvm.func.async)
                        {
                            var objf = new Vari("%CoroFrameType*", "%frame");
                            var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                            local.llvm.func.comps.Add(new Load(gj, objp));
                        }
                        else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(gj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                        else local.llvm.func.comps.Add(new Load(gj, local.blocks.Last().obj.obj.bas));
                        local.llvm.func.comps.Add(new Store(v01, gj));
                        if (local.llvm.func.async)
                        {
                            var objf = new Vari("%CoroFrameType*", "%frame");
                            objptr31 = new Vari(elif.model, "%obj" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", objptr31, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                            local.llvm.func.comps.Add(new Store(objptr31, go_c1));
                        }
                        else {
                            objptr31 = new Vari(elif.model + "**", "%objptr" + LLVM.n++);
                            var alloca1 = new Alloca(objptr31);
                            local.llvm.func.comps.Add(alloca1);
                            var objstore1 = new Store(objptr31, go_c1);
                            local.llvm.func.comps.Add(objstore1);
                            elif.bas = objptr31;

                            var srcall1 = new Call(null, srv, elif.bas);
                            local.llvm.func.comps.Add(srcall1);
                        }

                        int count1 = 8;
                        int order1 = 1;
                        foreach (var kv in blk10.vmapA)
                        {
                            var v = kv.Value;
                            if (v.type == ObjType.Variable)
                            {
                                var variable = v as Variable;
                                variable.order = order1++;
                                if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                if (variable.cls == local.Int)
                                {
                                    typedec.comps.Add(new TypeVal("i32", kv.Key));
                                    count1 += 8;
                                }
                                else if (variable.cls == local.Bool)
                                {
                                    typedec.comps.Add(new TypeVal("i1", kv.Key));
                                    count1 += 8;
                                }
                                else
                                {
                                    if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                                    vari = new Vari("i8*", "%v" + LLVM.n++);
                                    typedec1.comps.Add(new TypeVal("%" + variable.cls.letter.name + "Type*", kv.Key));

                                    gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod = new Load(vari2, vari);
                                    funccheck.comps.Add(co_lod);
                                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call = new Call(co_cval, co_val, thgc21, vari);
                                    funccheck.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck.comps.Add(store);
                                    count1 += 8;
                                }
                            }
                            else if (v.type == ObjType.Function)
                            {
                                var f = v as Function;
                                f.order = order1++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c1, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);

                            }
                            else if (v.type == ObjType.ClassObj)
                            {
                                var f = v as ClassObj;
                                f.order = order1++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c1, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);
                            }
                        }
                        foreach (var kv in blk2.vmapA)
                        {
                            var v = kv.Value;
                            if (v.type == ObjType.Variable)
                            {
                                var variable = v as Variable;
                                variable.order = order1++;
                                if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                if (variable.cls == local.Int)
                                {
                                    typedec.comps.Add(new TypeVal("i32", kv.Key));
                                    count += 8;
                                }
                                else if (variable.cls == local.Bool)
                                {
                                    typedec.comps.Add(new TypeVal("i2", kv.Key));
                                    count += 8;
                                }
                                else
                                {
                                    if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                                    vari1 = new Vari("i8*", "%v" + LLVM.n++);
                                    typedec1.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));

                                    gete1 = new Gete(elif.model, vari1, i8p1, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                                    vari12 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod1 = new Load(vari12, vari1);
                                    funccheck2.comps.Add(co_lod);
                                    co_cval1 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call1 = new Call(co_cval1, co_val1, thgc21, vari1);
                                    funccheck2.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck2.comps.Add(store);
                                    count1 += 8;
                                }
                            }
                            else if (v.type == ObjType.Function)
                            {
                                var f = v as Function;
                                f.order = order++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c1, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);

                            }
                            else if (v.type == ObjType.ClassObj)
                            {
                                var f = v as ClassObj;
                                f.order = order++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c1, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);
                            }
                        }
                        local.blok = go_c1;
                        local.typedec = typedec1;
                        local.funccheck = funccheck2;
                        local.funcdec = local.llvm.func;
                        local.i8p = i8p;
                        local.objptr3 = objptr31;
                        local.co_val = co_val;
                        local.countv = countv1;
                        local.odr = order;
                        local.count = count1;
                        for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                        decs = new List<Obj>();
                        funccheck2.comps.Add(new Ret(new Vari("void", null)));

                        local.blocks.Add(blk0);
                        blk10.exeC(local);

                        var lab11 = new Lab("then" + LLVM.n++);
                        lab2 = new Lab("else" + LLVM.n++);
                        if (calls.Count == m + 1) lab2 = end;
                        var br1 = new Br(blk10.rets.Last().vari, lab11, lab2);
                        elif.entry = lab11;
                        elif.end = end;
                        local.llvm.func.comps.Add(br1);
                        local.llvm.func.comps.Add(lab11);
                        local.blocks.Add(blk12);
                        local.labs.Add(lab11);
                        blk12.exeC(local);
                        local.labs.RemoveAt(local.labs.Count - 1);
                        local.blocks.RemoveAt(local.blocks.Count - 1);
                        local.blocks.RemoveAt(local.blocks.Count - 1);
                        if (!values.ContainsKey(lab11.name))
                        {
                            if (blk12.rets.Last().type == ObjType.Value)
                            {
                                var value = blk12.rets.Last() as Value;
                                var va = Bitcast.Cast(local, ret, value.cls, blk12.vari);
                                values[lab11.name] = new IfValue(lab11, va);
                                local.llvm.func.comps.Add(new Br(null, end));
                            }
                            else if (blk12.rets.Last().type == ObjType.VoiVal)
                            {
                                local.llvm.func.comps.Add(new Br(null, end));
                            }
                            else return Obj.Error(ObjType.Error, letter, "ifの値がありません");
                        }

                        local.llvm.func.comps.Add(new Br(null, end));
                        m++;
                        if (calls.Count != m)
                        {
                            local.llvm.func.comps.Add(lab2);
                        }
                        if (ret.cls == null) return Obj.Error(ObjType.Wait, letter, "varの型が判明しません");
                        n++;
                        val2 = primary.children[n];
                        goto head;
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "elif文の中身が宣言されていません。");

                }
                else if (val2.type == ObjType.Else)
                {
                    if (local.llvm.func.async)
                    {
                        var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                        var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(objv, objp));
                        var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                        var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(blockv, blockptr));
                        local.llvm.func.comps.Add(new Store(objp, blockv));
                    }
                    else
                    {
                        var rnpv = new Vari("i32*", "%ptr");
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v");
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                    }
                    var val = val2;
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.CallBlock)
                    {
                        var blk10 = val2.children[0] as Block;
                        var blk12 = val2.children[1] as Block;
                        this.n = Obj.cn++;
                        elif.model = "%" + elif.letter.name + "ElseType" + LLVM.n++;
                        var typedec1 = new TypeDec(elif.model);
                        local.llvm.types.Add(typedec1);
                        typedec1.comps.Add(new TypeVal("i8*", "blk"));

                        var i8p1 = new Vari("i8*", "%self");
                        var thgc21 = new Vari("%ThreadGCType*", "%thgc");
                        checkname = "@" + elif.letter.name + "ElseCheck" + LLVM.n++;
                        var funccheck2 = new Func(local.llvm, new Vari("void", checkname), thgc21, i8p1);
                        local.llvm.comps.Add(funccheck2);
                        var vari1 = new Vari("i8*", "%v" + LLVM.n++);
                        var gete1 = new Gete(elif.model, vari1, i8p1, new Vari("i32", "0"), new Vari("i32", "0"));
                        funccheck2.comps.Add(gete1);
                        var co_val1 = new Vari("%CopyObjectType", "%co");
                        var co_load1 = new Load(co_val1, new Vari("%CopyObjectType*", "@CopyObject"));
                        funccheck2.comps.Add(co_load1);
                        var vari12 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_lod1 = new Load(vari12, vari1);
                        funccheck2.comps.Add(co_lod1);
                        var co_cval1 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_call1 = new Call(co_cval1, thgc21, co_val1, vari1);
                        funccheck2.comps.Add(co_call1);
                        var store1 = new Store(vari1, co_cval1);
                        funccheck2.comps.Add(store1);

                        var ac_val1 = new Vari("%GC_AddClassType", "%addclass");
                        var countv1 = new Vari("i32", "");
                        var name1 = "else" + LLVM.n++;
                        var strv1 = new StrV("@" + name, name, name.Length * 1);
                        local.llvm.main.comps.Add(ac_call);
                        typ = new Vari("i32", (elif.n = local.llvm.cn++).ToString());
                        tnp = new Vari("i32", "%tnp" + LLVM.n++);
                        local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                        tv = new Vari("i32", "%cv" + LLVM.n++);
                        local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                        var ac_call1 = new Call(null, ac_val1, thgc3, countv1, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                        local.llvm.main.comps.Add(ac_call);

                        var go_v1 = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
                        tmp = new Vari("i32", elif.n.ToString());
                        tmi = new Vari("i32", "%tmi" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                        tmv = new Vari("i32", "%tmv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
                        var go_call1 = new Call(go_v1, go_val, thgc3, tmv);
                        local.llvm.func.comps.Add(go_call1);
                        var go_c1 = new Vari(elif.model + "*", "%v" + LLVM.n++);
                        var go_cast1 = new Bitcast(go_c1, go_v1);
                        local.llvm.func.comps.Add(go_cast1);

                        Vari objptr31;

                        var v01 = new Vari("i8**", "%v" + LLVM.n++);
                        gete = new Gete(elif.model, v01, go_c1, new Vari("i32", "0"), new Vari("i32", "0"));
                        local.llvm.func.comps.Add(gete); ;

                        gj = new Vari("%GCObjectPtr", "%gj" + LLVM.n++);
                        if (local.llvm.func.async)
                        {
                            var objf = new Vari("%CoroFrameType*", "%frame");
                            var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                            local.llvm.func.comps.Add(new Load(gj, objp));
                        }
                        else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(gj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                        else local.llvm.func.comps.Add(new Load(gj, local.blocks.Last().obj.obj.bas));
                        local.llvm.func.comps.Add(new Store(v01, gj));


                        if (local.llvm.func.async)
                        {
                            var objf = new Vari("%CoroFrameType*", "%frame");
                            objptr31 = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", objptr31, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                            local.llvm.func.comps.Add(new Store(objptr31, go_c1));
                        }
                        else {
                            objptr31 = new Vari(elif.model + "**", "%objptr" + LLVM.n++);
                            var alloca1 = new Alloca(objptr31);
                            local.llvm.func.comps.Add(alloca1);
                            var objstore1 = new Store(objptr31, go_c1);
                            local.llvm.func.comps.Add(objstore1);
                            elif.bas = objptr31;

                            var srv1 = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                            var srcall1 = new Call(null, srv, elif.bas);
                            local.llvm.func.comps.Add(srcall1);
                        }
                        int count1 = 8;
                        int order1 = 1;
                        foreach (var kv in blk10.vmapA)
                        {
                            var v = kv.Value;
                            if (v.type == ObjType.Variable)
                            {
                                var variable = v as Variable;
                                variable.order = order1++;
                                if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                if (variable.cls == local.Int)
                                {
                                    typedec1.comps.Add(new TypeVal("i32", kv.Key));
                                    count1 += 8;
                                }
                                else if (variable.cls == local.Bool)
                                {
                                    typedec1.comps.Add(new TypeVal("i2", kv.Key));
                                    count1 += 8;
                                }
                                else
                                {
                                    if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                                    vari1 = new Vari("i8*", "%v" + LLVM.n++);
                                    typedec1.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));

                                    gete1 = new Gete(elif.model, vari1, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                                    vari12 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod1 = new Load(vari2, vari);
                                    funccheck2.comps.Add(co_lod1);
                                    co_cval1 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call1 = new Call(co_cval1, co_val1, thgc21, vari1);
                                    funccheck2.comps.Add(co_call);
                                    store1 = new Store(vari1, co_cval1);
                                    funccheck2.comps.Add(store);
                                    count1 += 8;
                                }
                            }
                            else if (v.type == ObjType.Function)
                            {
                                var f = v as Function;
                                if (f.identity == 0) f.identity = LLVM.n++;
                                f.order = order1++;
                                typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);

                            }
                            else if (v.type == ObjType.ClassObj)
                            {
                                var f = v as ClassObj;
                                if (f.identity == 0) f.identity = LLVM.n++;
                                f.order = order1++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);
                            }
                        }
                        foreach (var kv in blk2.vmapA)
                        {
                            var v = kv.Value;
                            if (v.type == ObjType.Variable)
                            {
                                var variable = v as Variable;
                                variable.order = order1++;
                                if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                if (variable.cls == local.Int)
                                {
                                    typedec1.comps.Add(new TypeVal("i32", kv.Key));
                                    count1 += 8;
                                }
                                else if (variable.cls == local.Bool)
                                {
                                    typedec1.comps.Add(new TypeVal("i2", kv.Key));
                                    count1 += 8;
                                }
                                else
                                {
                                    if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                                    vari1 = new Vari("i8*", "%v" + LLVM.n++);
                                    typedec1.comps.Add(new TypeVal("%" + variable.cls.letter.name + "Type*", kv.Key));

                                    gete1 = new Gete(elif.model, vari1, i8p1, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                                    vari12 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod1 = new Load(vari12, vari1);
                                    funccheck2.comps.Add(co_lod);
                                    co_cval1 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call1 = new Call(co_cval1, co_val1, thgc21, vari1);
                                    funccheck2.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck2.comps.Add(store);
                                    count1 += 8;
                                }
                            }
                            else if (v.type == ObjType.Function)
                            {
                                var f = v as Function;
                                if (f.identity == 0) f.identity = LLVM.n++;
                                f.order = order++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);

                            }
                            else if (v.type == ObjType.ClassObj)
                            {
                                var f = v as ClassObj;
                                if (f.identity == 0) f.identity = LLVM.n++;
                                f.order = order++;
                                typedec1.comps.Add(new TypeVal("%FuncType*", kv.Key));
                                decs.Add(f);

                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                funccheck2.comps.Add(gete);
                                vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                co_lod = new Load(vari2, vari);
                                funccheck2.comps.Add(co_lod);
                                co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                co_call = new Call(co_cval, co_val, thgc21, vari);
                                funccheck2.comps.Add(co_call);
                                store = new Store(vari, co_cval);
                                funccheck2.comps.Add(store);
                                count1 += 8;

                                var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                local.llvm.func.comps.Add(fu_call);
                                var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                local.llvm.comps.Add(new Load(va2, objptr31));
                                var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                gete = new Gete(elif.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                local.llvm.func.comps.Add(gete);
                                var fu_store = new Store(va, va0);
                                local.llvm.func.comps.Add(fu_store);
                            }
                        }
                        local.blok = go_c1;
                        local.typedec = typedec1;
                        local.funccheck = funccheck2;
                        local.funcdec = local.llvm.func;
                        local.i8p = i8p;
                        local.objptr3 = objptr31;
                        local.co_val = co_val;
                        local.countv = countv1;
                        local.odr = order;
                        local.count = count1;
                        for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                        decs = new List<Obj>();
                        countv1.name = count1.ToString();
                        funccheck2.comps.Add(new Ret(new Vari("void", null)));
                        elif.entry = lab2;
                        elif.end = end;
                        local.labs.Add(lab2);
                        local.blocks.Add(blk0);
                        blk10.exeC(local);
                        local.blocks.Add(blk12);
                        blk12.exeC(local);
                        local.labs.RemoveAt(local.labs.Count - 1);
                        local.blocks.RemoveAt(local.blocks.Count - 1);
                        local.blocks.RemoveAt(local.blocks.Count - 1);

                        if (!values.ContainsKey(lab2.name))
                        {
                            if (blk12.rets.Last().type == ObjType.Value)
                            {
                                var value = blk12.rets.Last() as Value;
                                var va = Bitcast.Cast(local, ret, value.cls, blk12.vari);
                                values[lab2.name] = new IfValue(lab2, va);
                                local.llvm.func.comps.Add(new Br(null, end));
                            }
                            else if (blk12.rets.Last().type == ObjType.VoiVal)
                            {
                                local.llvm.func.comps.Add(new Br(null, end));
                            }
                            else return Obj.Error(ObjType.Error, letter, "ifの値がありません");
                        }
                        local.labs.RemoveAt(local.labs.Count - 1);
                        local.labs.Add(end);
                        if (local.llvm.func.async)
                        {
                            var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                            var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(objv, objp));
                            var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                            var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(blockv, blockptr));
                            local.llvm.func.comps.Add(new Store(objp, blockv));
                        }
                        else
                        {
                            var rnpv = new Vari("i32*", "%ptr");
                            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                            var rnpv12 = new Vari("i32", "%v");
                            var rnp12load = new Load(rnpv12, rnpv);
                            local.llvm.func.comps.Add(rnp12load);
                            var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                            local.llvm.func.comps.Add(rnpsub);
                            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        }
                        local.llvm.func.comps.Add(end);
                        n++;
                        if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                        if (ret == local.Void)
                        {
                            return new Value(ret);
                        }
                        else
                        {
                            if (ret == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
                            else if (ret == local.Bool) vari = new Vari("i1", "%v" + LLVM.n++);
                            else vari = new Vari("%" + ret.cls.letter.name + "Type", "%v" + LLVM.n++);
                            var phi = new Phi(vari, values);
                            local.llvm.func.comps.Add(phi);
                            return new Value(ret, vari);
                        }
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "else文の中身が宣言されていません。");
                }
                else
                {
                    local.llvm.func.comps.Add(end);
                    if (local.llvm.func.async)
                    {
                        var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                        var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(objv, objp));
                        var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                        var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(blockv, blockptr));
                        local.llvm.func.comps.Add(new Store(objp, blockv));
                    }
                    else
                    {
                        var rnpv = new Vari("i32*", "%ptr");
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v");
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                    }
                    local.labs.RemoveAt(local.labs.Count - 1);
                    local.labs.Add(end);
                    if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                    if (ret == local.Void)
                    {
                        return new Value(ret);
                    }
                    else {
                        if (ret == local.Int) vari = new Vari("i32", "%v" + LLVM.n++);
                        else if (ret == local.Bool) vari = new Vari("i1", "%v" + LLVM.n++);
                        else vari = new Vari("%" + ret.cls.letter.name + "Type", "%v" + LLVM.n++);
                        var phi = new Phi(vari, values);
                        local.llvm.func.comps.Add(phi);
                        return new Value(ret, vari);
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "if文の中身が宣言されていません");
        }
    }
    partial class While
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.CallBlock)
            {
                var blk0 = val2.children[0] as Block;
                var blk2 = val2.children[1] as Block;
                n++;

                val2 = primary.children[n];
                this.n = Obj.cn++;
                this.model = "%" + this.letter.name + "WhileType" + LLVM.n++;
                var typedec = new TypeDec(this.model);
                local.llvm.types.Add(typedec);

                var i8p = new Vari("i8*", "%self");
                var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                var checkname = "@" + this.letter.name + "WhileCheck" + LLVM.n++;
                var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
                local.llvm.comps.Add(funccheck);
                typedec.comps.Add(new TypeVal("i8*", "blk"));
                var vari = new Vari("i8*", "%v" + LLVM.n++);
                var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
                funccheck.comps.Add(gete);
                var co_val = new Vari("%CopyObjectType", "%co");
                var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
                funccheck.comps.Add(co_load);
                var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                var co_lod = new Load(vari2, vari);
                funccheck.comps.Add(co_lod);
                var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                var co_call = new Call(co_cval, co_val, thgc2, vari);
                funccheck.comps.Add(co_call);
                var store = new Store(vari, co_cval);
                funccheck.comps.Add(store);

                var thgc3 = new Vari("%ThreadGCType*", "%thgc");
                var ac_val = new Vari("%GC_AddClassType", "%addclass");
                var countv = new Vari("i32", "");
                var name = "while" + LLVM.n++;
                var strv = new StrV("@" + name, name, name.Length * 1);
                local.llvm.strs.Add(strv);
                var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
                var tnp = new Vari("i32", "%tnp" + LLVM.n++);
                local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                var tv = new Vari("i32", "%cv" + LLVM.n++);
                local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                var ac_call = new Call(null, ac_val, thgc3, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                local.llvm.main.comps.Add(ac_call);

                if (local.llvm.func.async)
                {
                    //checkstate Br Checks in loops
                }
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                var go_val = new Vari("%GC_mallocType", "%gcobject" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(go_val, new Vari("%GC_mallocType*", "@GC_malloc")));
                var go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
                var tmp = new Vari("i32", this.n.ToString());
                var tmi = new Vari("i32", "%tmi" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                var tmv = new Vari("i32", "%tmv" + LLVM.n++);
                local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
                var go_call = new Call(go_v, go_val, thgc4, tmv);
                local.llvm.func.comps.Add(go_call);
                var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
                var go_cast = new Bitcast(go_c, go_v);
                local.llvm.func.comps.Add(go_cast);

                var v0 = new Vari("i8**", "%v" + LLVM.n++);
                gete = new Gete(this.model, v0, go_c, new Vari("i32", "0"), new Vari("i32", "0"));
                local.llvm.func.comps.Add(gete);

                var gj = new Vari("%GCObjectPtr", "%gj" + LLVM.n++);
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Load(gj, objp));
                }
                else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(gj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                else local.llvm.func.comps.Add(new Load(gj, local.blocks.Last().obj.obj.bas));
                local.llvm.func.comps.Add(new Store(v0, gj));


                var rn4 = new Vari("%RootNodeType*", "%rn");
                var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                Vari objptr3;
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    objptr3 = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objptr3, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Store(objptr3, go_c));
                }
                else {
                    objptr3 = new Vari(this.model + "**", "%objptr" + LLVM.n++);
                    var alloca = new Alloca(objptr3);
                    local.llvm.func.comps.Add(alloca);
                    var objstore = new Store(objptr3, go_c);
                    local.llvm.func.comps.Add(objstore);
                    this.bas = objptr3;

                    var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
                    local.llvm.func.comps.Add(srload);
                    var srcall = new Call(null, srv, rn4, this.bas);
                    local.llvm.func.comps.Add(srcall);
                }
                int count = 8;
                int order = 1;
                var decs = new List<Obj>();
                foreach (var kv in blk0.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal("%" + variable.cls.letter.name + "Type*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                local.blok = go_c;
                local.typedec = typedec;
                local.funccheck = funccheck;
                local.funcdec = local.llvm.func;
                local.i8p = i8p;
                local.objptr3 = objptr3;
                local.co_val = co_val;
                local.countv = countv;
                local.odr = order;
                local.count = count;
                for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                decs = new List<Obj>();
                Vari pv = null;
                Vari vv = null;
                Dictionary<String, IfValue> ifv = new Dictionary<String, IfValue>();
                this.ifv = ifv;
                if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                if (ret != local.Void)
                {
                    var vcl = new Vari("%CreateHashType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(vcl, new Vari("%CreateHashType*", "@CreateHash")));
                    pv = new Vari("%HashType*", "%v" + LLVM.n++);
                    forvari = pv;
                    int size = 8;
                    bool objected = true;
                    if (ret == local.Int)
                    {
                        size = 4;
                        objected = false;
                    }
                    else if (ret == local.Bool)
                    {
                        size = 1;
                        objected = false;
                    }
                    local.llvm.func.comps.Add(new Call(pv, vcl, thgc4, new Vari("i1", objected.ToString().ToLower())));

                    var variable = new Variable(ret);
                    variable.order = order++;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", "ret"));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", "ret"));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", "ret"));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                    }
                    var va = new Vari("%HashType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                    local.llvm.func.comps.Add(new Store(va, pv));
                }

                countv.name = count.ToString();
                funccheck.comps.Add(new Ret(new Vari("void", null)));
                local.blocks.Add(blk0);
                blk0.exeRangeC(0, blk0.children.Count - 1, local, false);
                Lab lab = new Lab("cond");
                this.entry = lab;
                local.llvm.func.comps.Add(new Br(null, lab));
                local.llvm.func.comps.Add(lab);
                local.llvm.func.comps.Add(new Phi(pv, ifv));
                local.labs.Add(lab);
                var obj = blk0.children[blk0.children.Count - 1].exeC(local);
                local.labs.RemoveAt(local.labs.Count - 1);
                var lab1 = new Lab("loop");
                var lab2 = new Lab("end");
                this.entry = lab2;
                var br = new Br(obj.vari, lab1, lab2);
                local.llvm.func.comps.Add(br);
                local.llvm.func.comps.Add(lab1);
                local.blocks.Add(blk2);
                local.labs.Add(lab1);
                blk2.exeC(local);
                local.labs.RemoveAt(local.labs.Count - 1);
                if (!ifv.ContainsKey(lab1.name))
                {
                    if (ret != local.Void)
                    {
                        if (blk2.rets.Last().type == ObjType.Value)
                        {
                            var value = blk2.rets.Last() as Value;
                            var va = Bitcast.Cast(local, ret, value.cls, blk2.rets.Last().vari);
                            var varih = new Vari("%AddHashType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(varih, new Vari("%AddHashType*", "@AddHash")));
                            local.llvm.func.comps.Add(new Call(null, varih, thgc4, new Vari("%StringType*", "null"), pv, va));
                            local.llvm.func.comps.Add(new Br(null, lab));
                        }
                    }
                }
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);

                local.llvm.func.comps.Add(lab2);
                local.labs.RemoveAt(local.labs.Count - 1);
                local.labs.Add(lab2);

                if (local.llvm.func.async)
                {
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                    var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(objv, objp));
                    var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                    var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(blockv, blockptr));
                    local.llvm.func.comps.Add(new Store(objp, blockv));
                }
                else {
                    var rnpv = new Vari("i32*", "%ptr");
                    local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                    var rnpv12 = new Vari("i32", "%v");
                    var rnp12load = new Load(rnpv12, rnpv);
                    local.llvm.func.comps.Add(rnp12load);
                    var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                    var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                    local.llvm.func.comps.Add(rnpsub);
                    local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                }
                n++;
                return new Value(ret, pv);
            }
            return Obj.Error(ObjType.Error, val2.letter, "while文の中身が宣言されていません");
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
    }
    partial class For
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.CallBlock)
            {
                var blk0 = val2.children[0] as Block;
                var blk2 = val2.children[1] as Block;
                n++;

                val2 = primary.children[n];
                this.n = Obj.cn++;
                this.model = "%" + this.letter.name + "ForType" + LLVM.n++;
                var typedec = new TypeDec(this.model);
                local.llvm.types.Add(typedec);

                var i8p = new Vari("i8*", "%self");
                var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                var checkname = "@" + this.letter.name + "ForCheck" + LLVM.n++;
                var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
                local.llvm.comps.Add(funccheck);
                typedec.comps.Add(new TypeVal("i8*", "blk"));
                var vari = new Vari("i8*", "%v" + LLVM.n++);
                var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
                funccheck.comps.Add(gete);
                var co_val = new Vari("%CopyObjectType", "%co");
                var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
                funccheck.comps.Add(co_load);
                var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                var co_lod = new Load(vari2, vari);
                funccheck.comps.Add(co_lod);
                var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                var co_call = new Call(co_cval, co_val, thgc2, vari);
                funccheck.comps.Add(co_call);
                var store = new Store(vari, co_cval);
                funccheck.comps.Add(store);

                var thgc3 = new Vari("%ThreadGCType*", "%thgc");
                var ac_val = new Vari("%GC_AddClassType", "%addclass");
                var countv = new Vari("i32", "");
                var name = "for" + LLVM.n++;
                var strv = new StrV("@" + name, name, name.Length * 1);
                local.llvm.strs.Add(strv);
                var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
                var tnp = new Vari("i32", "%tnp" + LLVM.n++);
                local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                var tv = new Vari("i32", "%cv" + LLVM.n++);
                local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                var ac_call = new Call(null, ac_val, thgc3, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                local.llvm.main.comps.Add(ac_call);

                if (local.llvm.func.async)
                {
                    //checkstate Br checks in loops
                }
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                var go_val = new Vari("%GC_mallocType", "%gcobject" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(go_val, new Vari("%GC_mallocType*", "@GC_malloc")));
                var go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
                var tmp = new Vari("i32", this.n.ToString());
                var tmi = new Vari("i32", "%tmi" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                var tmv = new Vari("i32", "%tmv" + LLVM.n++);
                local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
                var go_call = new Call(go_v, go_val, thgc4, tmv);
                local.llvm.func.comps.Add(go_call);
                var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
                var go_cast = new Bitcast(go_c, go_v);
                local.llvm.func.comps.Add(go_cast);

                var v0 = new Vari("i8**", "%v" + LLVM.n++);
                gete = new Gete(this.model, v0, go_c, new Vari("i32", "0"), new Vari("i32", "0"));
                local.llvm.func.comps.Add(gete);

                var gj = new Vari("%GCObjectPtr", "%gj" + LLVM.n++);
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Load(gj, objp));
                }
                else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(gj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                else local.llvm.func.comps.Add(new Load(gj, local.blocks.Last().obj.obj.bas));
                local.llvm.func.comps.Add(new Store(v0, gj));

                var rn4 = new Vari("%RootNodeType*", "%rn");
                var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                Vari objptr3;
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    objptr3 = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objptr3, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Store(objptr3, go_c));
                }
                else {
                    objptr3 = new Vari(this.model + "**", "%objptr" + LLVM.n++);
                    var alloca = new Alloca(objptr3);
                    local.llvm.func.comps.Add(alloca);
                    var objstore = new Store(objptr3, go_c);
                    local.llvm.func.comps.Add(objstore);
                    this.bas = objptr3;

                    var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
                    local.llvm.func.comps.Add(srload);
                    var srcall = new Call(null, srv, rn4, this.bas);
                    local.llvm.func.comps.Add(srcall);
                }
                int count = 8;
                int order = 1;
                var decs = new List<Obj>();
                foreach (var kv in blk0.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal("%" + variable.cls.letter.name + "Type*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                local.blok = go_c;
                local.typedec = typedec;
                local.funccheck = funccheck;
                local.funcdec = local.llvm.func;
                local.i8p = i8p;
                local.objptr3 = objptr3;
                local.co_val = co_val;
                local.countv = countv;
                local.odr = order;
                local.count = count;
                for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                decs = new List<Obj>();
                Vari pv = null;
                Vari vv = null;
                Dictionary<String, IfValue> ifv = new Dictionary<String, IfValue>();
                this.ifv = ifv;
                if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                if (ret != local.Void)
                {
                    var vcl = new Vari("%CreateHashType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(vcl, new Vari("%CreateHashType*", "@CreateHash")));
                    pv = new Vari("%HashType*", "%v" + LLVM.n++);
                    forvari = pv;
                    int size = 8;
                    bool objected = true;
                    if (ret == local.Int)
                    {
                        size = 4;
                        objected = false;
                    }
                    else if (ret == local.Bool)
                    {
                        size = 1;
                        objected = false;
                    }
                    local.llvm.func.comps.Add(new Call(pv, vcl, thgc4, new Vari("i1", objected.ToString().ToLower())));
                    var variable = new Variable(ret);
                    variable.order = order++;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", "ret"));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", "ret"));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", "ret"));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                    }
                    var va = new Vari("%HashType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                    local.llvm.func.comps.Add(new Store(va, pv));

                }
                countv.name = count.ToString();
                funccheck.comps.Add(new Ret(new Vari("void", null)));
                local.blocks.Add(blk0);
                blk0.exeRangeC(0, blk0.children.Count - 2, local, false);
                Lab lab = new Lab("cond");
                this.entry = lab;
                local.llvm.func.comps.Add(new Br(null, lab));
                local.llvm.func.comps.Add(lab);
                local.labs.Add(lab);
                var obj = blk0.children[blk0.children.Count - 2].exeC(local);
                local.labs.RemoveAt(local.labs.Count - 1);
                var lab1 = new Lab("loop");
                var lab2 = new Lab("end");
                this.end = lab2;
                var br = new Br(obj.vari, lab1, lab2);
                local.llvm.func.comps.Add(br);
                local.llvm.func.comps.Add(lab1);
                local.blocks.Add(blk2);
                local.labs.Add(lab1);
                blk2.exeC(local);
                local.labs.RemoveAt(local.labs.Count - 1);
                blk0.children[blk0.children.Count - 1].exeC(local);
                if (!ifv.ContainsKey(lab1.name))
                {
                    if (ret != local.Void)
                    {
                        if (blk2.rets.Last().type == ObjType.Value)
                        {
                            var value = blk2.rets.Last() as Value;
                            var va = Bitcast.Cast(local, ret, value.cls, blk2.rets.Last().vari);
                            var varih = new Vari("%AddHashType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(varih, new Vari("%AddHashType*", "@AddHash")));
                            local.llvm.func.comps.Add(new Call(null, varih, thgc4, new Vari("%StringType*", "null"), pv, va));
                            local.llvm.func.comps.Add(new Br(null, lab));
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "for文の中身が宣言されていません。");
                    }
                }
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);

                local.llvm.func.comps.Add(lab2);
                local.labs.RemoveAt(local.labs.Count - 1);
                local.labs.Add(lab2);

                if (local.llvm.func.async)
                {
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                    var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(objv, objp));
                    var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                    var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(blockv, blockptr));
                    local.llvm.func.comps.Add(new Store(objp, blockv));
                }
                else {
                    var rnpv = new Vari("i32*", "%ptr");
                    local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                    var rnpv12 = new Vari("i32", "%v");
                    var rnp12load = new Load(rnpv12, rnpv);
                    local.llvm.func.comps.Add(rnp12load);
                    local.llvm.func.comps.Add(rnp12load);
                    var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                    var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                    local.llvm.func.comps.Add(rnpsub);
                    local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                }
                n++;
                return new Value(ret, pv);
            }
            return Obj.Error(ObjType.Error, val2.letter, "for文の中身が宣言されていません。");
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
    }
    partial class Switch
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.CallBlock)
            {
                var blk0 = val2.children[0] as Block;
                var blk2 = val2.children[1] as Block;
                n++;

                val2 = primary.children[n];
                this.n = Obj.cn++;
                this.model = "%" + this.letter.name + "SwitchType" + LLVM.n++;
                var typedec = new TypeDec(this.model);
                local.llvm.types.Add(typedec);

                var i8p = new Vari("i8*", "%self");
                var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                var checkname = "@" + this.letter.name + "SwitchCheck" + LLVM.n++;
                var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
                local.llvm.comps.Add(funccheck);
                typedec.comps.Add(new TypeVal("i8*", "blk"));
                var vari = new Vari("i8*", "%v" + LLVM.n++);
                var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
                funccheck.comps.Add(gete);
                var co_val = new Vari("%CopyObjectType", "%co");
                var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
                funccheck.comps.Add(co_load);
                var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                var co_lod = new Load(vari2, vari);
                funccheck.comps.Add(co_lod);
                var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                var co_call = new Call(co_cval, co_val, thgc2, vari);
                funccheck.comps.Add(co_call);
                var store = new Store(vari, co_cval);
                funccheck.comps.Add(store);

                var thgc3 = new Vari("%ThreadGCType*", "%thgc");
                var ac_val = new Vari("%GC_AddClassType", "%addclass");
                var countv = new Vari("i32", "");
                var name = "switch" + LLVM.n++;
                var strv = new StrV("@" + name, name, name.Length * 1);
                local.llvm.strs.Add(strv);
                var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
                var tnp = new Vari("i32", "%tnp" + LLVM.n++);
                local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                var tv = new Vari("i32", "%cv" + LLVM.n++);
                local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                var ac_call = new Call(null, ac_val, thgc3, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                local.llvm.main.comps.Add(ac_call);

                if (local.llvm.func.async)
                {
                    //checkstate Br checks in Switch
                }
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                var go_val = new Vari("%GC_mallocType", "%gcobject" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(go_val, new Vari("%GC_mallocType*", "@GC_malloc")));
                var go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
                var tmp = new Vari("i32", this.n.ToString());
                var tmi = new Vari("i32", "%tmi" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                var tmv = new Vari("i32", "%tmv" + LLVM.n++);
                local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
                var go_call = new Call(go_v, go_val, thgc4, tmv);
                local.llvm.func.comps.Add(go_call);
                var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
                var go_cast = new Bitcast(go_c, go_v);
                local.llvm.func.comps.Add(go_cast);

                var v0 = new Vari("i8**", "%v" + LLVM.n++);
                gete = new Gete(this.model, v0, go_c, new Vari("i32", "0"), new Vari("i32", "0"));
                local.llvm.func.comps.Add(gete);

                var gj = new Vari("%GCObjectPtr", "%gj" + LLVM.n++);
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Load(gj, objp));
                }
                else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(gj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                else local.llvm.func.comps.Add(new Load(gj, local.blocks.Last().obj.obj.bas));
                local.llvm.func.comps.Add(new Store(v0, gj));

                var rn4 = new Vari("%RootNodeType*", "%rn");
                var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                Vari objptr3;
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    objptr3 = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objptr3, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Store(objptr3, go_c));
                }
                else {
                    objptr3 = new Vari(this.model + "**", "%objptr" + LLVM.n++);
                    var alloca = new Alloca(objptr3);
                    local.llvm.func.comps.Add(alloca);
                    var objstore = new Store(objptr3, go_c);
                    local.llvm.func.comps.Add(objstore);
                    this.bas = objptr3;

                    var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
                    local.llvm.func.comps.Add(srload);
                    var srcall = new Call(null, srv, rn4, this.bas);
                    local.llvm.func.comps.Add(srcall);
                }
                int count = 8;
                int order = 1;
                var decs = new List<Obj>();
                foreach (var kv in blk0.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal("%" + variable.cls.letter.name + "Type*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                            count += 8;
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;
                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        local.llvm.func.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        local.llvm.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        local.llvm.func.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        local.llvm.func.comps.Add(fu_store);
                    }
                }
                local.blok = go_c;
                local.typedec = typedec;
                local.funccheck = funccheck;
                local.funcdec = local.llvm.func;
                local.i8p = i8p;
                local.objptr3 = objptr3;
                local.co_val = co_val;
                local.countv = countv;
                local.odr = order;
                local.count = count;
                for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                decs = new List<Obj>();
                countv.name = count.ToString();
                funccheck.comps.Add(new Ret(new Vari("void", null)));
                local.blocks.Add(blk0);
                blk0.exeC(local);
                Lab lab = new Lab("end");
                Vari pv = null;
                Vari vv = null;
                Dictionary<String, IfValue> ifv = new Dictionary<String, IfValue>();
                List<IfValue> ifv2 = new List<IfValue>();
                this.ifv = ifv;
                this.end = lab;
                int size = 64;
                Vari variy = blk0.rets.Last().vari;
                var obj = blk0.rets.Last();
                if (obj.type == ObjType.Value)
                {
                    var value = obj as Value;
                    if (value.cls == local.Str)
                    {
                        var gh = new Vari("%GetHashValueType", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(gh, new Vari("%GetHashValueType*", "@GetHashValue")));
                        var varix = new Vari("i32", "%v" + LLVM.n++);
                        local.llvm.func.comps.Add(new Call(varix, gh, variy, new Vari("i32", size.ToString())));
                        variy = varix;
                    }
                }
                local.llvm.func.comps.Add(new SwitchComp(variy, lab, ifv2));
                if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                if (ret != local.Void)
                {
                    if (ret == local.Int) pv = new Vari("i32", "%v" + LLVM.n++);
                    else if (ret == local.Bool) pv = new Vari("i1", "%v" + LLVM.n++);
                    else pv = new Vari(ret.model + "*", "%v" + LLVM.n++);
                    if (ret == local.Int) vv = new Vari("i32", "0");
                    else if (ret == local.Bool) vv = new Vari("i1", "false");
                    else vv = new Vari(ret.model + "*", "null");
                    ifv["entry"] = new IfValue(new Lab("entry"), vv);
                }
                if (ret.type == ObjType.Var) ret = (ret as Var).cls;
                local.blocks.Add(blk2);
                var larr = blk2.labelmap.Values.ToList();
                for (var i = 0; i < larr.Count; i++)
                {
                    Lab labi = new Lab("l" + larr[i].name);
                    if (obj.type == ObjType.Value)
                    {
                        var value = obj as Value;
                        if (value.cls == local.Int)
                        {
                            ifv2.Add(new IfValue(labi, ToVari(larr[i].name, value.cls, local, size)));
                        }
                        else if (value.cls == local.Str)
                        {
                            ifv2.Add(new IfValue(labi, ToVari(larr[i].name, value.cls, local, size)));
                        }
                    }
                    local.llvm.func.comps.Add(labi);
                    local.labs.Add(labi);
                    if (i == larr.Count - 1) blk2.exeRangeC(larr[i].n, blk2.children.Count, local, false);
                    else blk2.exeRangeC(larr[i].n, larr[i + 1].n, local, false);
                    local.labs.RemoveAt(local.labs.Count - 1);
                    if (!ifv.ContainsKey(labi.name))
                    {
                        if (blk2.rets.Last().type == ObjType.Value)
                        {
                            var value = blk2.rets.Last() as Value;
                            var va = Bitcast.Cast(local, ret, value.cls, blk2.rets.Last().vari);
                            ifv[labi.name] = new IfValue(labi, va);
                            local.llvm.func.comps.Add(new Br(null, lab));
                        }
                    }
                }
                local.llvm.func.comps.Add(lab);
                local.labs.RemoveAt(local.labs.Count - 1);
                local.labs.Add(lab);
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.llvm.func.comps.Add(new Phi(pv, ifv));


                if (local.llvm.func.async)
                {
                    var objp = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", objp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                    var objv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(objv, objp));
                    var blockptr = new Vari("%GCObjectPtr*", "%objp" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%GCObject", blockptr, objv, new Vari("i32", "0"), new Vari("i32", "0")));
                    var blockv = new Vari("%GCObjectPtr", "%objv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(blockv, blockptr));
                    local.llvm.func.comps.Add(new Store(objp, blockv));
                }
                else {
                    var rnpv = new Vari("i32*", "%ptr");
                    local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                    var rnpv12 = new Vari("i32", "%v");
                    var rnp12load = new Load(rnpv12, rnpv);
                    local.llvm.func.comps.Add(rnp12load);
                    var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                    var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                    local.llvm.func.comps.Add(rnpsub);
                    local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                }
                n++;
                return new Value(ret, pv);
            }
            return Obj.Error(ObjType.Error, val2.letter, "switch文の中身が宣言されていません");
        }
        public Vari ToVari(String name, Type cls, Local local, int size)
        {
            if (cls == local.Int)
            {
                if (int.TryParse(name, out int result))
                {
                    return new Vari("i32", name);
                }
                else throw new Exception("");
            }
            else if (cls == local.Str)
            {
                return new Vari("i32", get_hashvalue(name, size).ToString());
            }
            else throw new Exception();
        }
        int get_hashvalue(String key, int size)
        {
            int hash = 10000;
            for (int i = 0; i < key.Length; i++)
            {
                hash ^= key[i];
                hash *= 1619;
                hash %= size;
            }
            return hash;
        }
    }
    partial class Break
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            n++;
            Obj val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            if (val2 != null) val2 = val2.GetterC(local);
            var count = 0;
            for (var i = local.blocks.Count - 1; i >= 0; i--)
            {
                var block = local.blocks[i];
                if (block.obj != null)
                {
                    if (block.obj.obj.type == ObjType.IfBlock || block.obj.n == 0)
                    {
                        count++;
                    }
                    else if (block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For || block.obj.obj.type == ObjType.Switch)
                    {
                        Type ret;
                        if (block.obj.obj.type == ObjType.While) ret = (block.obj.obj as While).ret;
                        else if (block.obj.obj.type == ObjType.For) ret = (block.obj.obj as For).ret;
                        else ret = (block.obj.obj as Switch).ret;
                        var rnpv = new Vari("i32*", "%ptr" + LLVM.n++);
                        var rnpload = new Load(rnpv, new Vari("i32**", "@rnp"));
                        local.llvm.func.comps.Add(rnpload);
                        var rnpv12 = new Vari("i32", "%v" + LLVM.n++);
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", count.ToString()));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        Lab end = block.obj.obj.end;
                        if (block.obj.obj.type == ObjType.IfBlock)
                        {
                            end = (block.obj.obj as IfBlock).iflabel.end;
                        }
                        var lab = local.labs.Last();
                        if (val2 == null)
                        {
                            val2 = new VoiVal();
                            local.llvm.func.comps.Add(new Br(null, end));
                        }
                        if (val2.type == ObjType.Value)
                        {
                            var value = val2 as Value;
                            if (!block.obj.obj.ifv.ContainsKey(lab.name))
                            {
                                if (block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For)
                                {
                                    var v = val2.vari;
                                    if (value.cls == ret) block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                    else if (ret == local.Int)
                                    {
                                        v = new Vari("i32", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                        block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                    }
                                    else if (ret == local.Short)
                                    {
                                        v = new Vari("i16", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                        block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                    }
                                    else if (ret == local.Bool)
                                    {
                                        v = new Vari("i1", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                        block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                    }
                                    else block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    var varih = new Vari("%AddHashType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(varih, new Vari("%AddHashType*", "@AddHash")));
                                    local.llvm.func.comps.Add(new Call(null, varih, thgc4, new Vari("%StringType*", "null"), block.obj.obj.forvari, v));
                                }
                                else if (value.cls == ret) block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                else if (ret == local.Int)
                                {
                                    var v = new Vari("i32", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                }
                                else if (ret == local.Short)
                                {
                                    var v = new Vari("i16", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                }
                                else if (ret == local.Bool)
                                {
                                    var v = new Vari("i1", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                }
                                else block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);

                            }
                            local.llvm.func.comps.Add(new Br(null, end));
                        }
                        var lastblock = local.blocks.Last();
                        if (!lastblock.obj.obj.ifv.ContainsKey(lab.name)) lastblock.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), null);
                        n--;
                        return this;
                    }
                    else if (block.obj.obj.type == ObjType.Function || block.obj.obj.type == ObjType.ClassObj || block.obj.obj.type == ObjType.ModelObj || block.obj.obj.type == ObjType.GeneObj || block.obj.obj.type == ObjType.GenericFunction)
                    {
                        return Obj.Error(ObjType.Error, letter, "while,for,switchの中ではありません");
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "while,for,switchの中ではありません");
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            if (val2 != null) val2 = val2.GetterC(local);
            var count = 0;
            for (var i = local.blocks.Count - 1; i >= 0; i--)
            {
                var block = local.blocks[i];
                if (block.obj != null)
                {
                    if (block.obj.obj.type == ObjType.IfBlock || block.obj.n == 0)
                    {
                        count++;
                    }
                    else if (block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For || block.obj.obj.type == ObjType.Switch)
                    {
                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var rnpv = new Vari("i32*", "%ptr" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v" + LLVM.n++);
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", count.ToString()));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        Lab end = block.obj.obj.end;
                        if (block.obj.obj.type == ObjType.IfBlock)
                        {
                            end = (block.obj.obj as IfBlock).iflabel.end;
                        }
                        var lab = local.labs.Last();
                        if (val2 == null)
                        {
                            val2 = new VoiVal();
                            local.llvm.func.comps.Add(new Br(null, end));
                        }
                        if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value || val2.type == ObjType.Block)
                        {
                            value = val2;
                            if (!block.obj.obj.ifv.ContainsKey(lab.name)) block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                            local.llvm.func.comps.Add(new Br(null, end));
                        }
                        var lastblock = local.blocks.Last();
                        if (!lastblock.obj.obj.ifv.ContainsKey(lab.name)) lastblock.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), null);
                        return this;
                    }
                    else if (block.obj.obj.type == ObjType.Function || block.obj.obj.type == ObjType.ClassObj || block.obj.obj.type == ObjType.ModelObj || block.obj.obj.type == ObjType.GeneObj || block.obj.obj.type == ObjType.GenericFunction)
                    {
                        return Obj.Error(ObjType.Error, letter, "while,for,switchの中ではありません");
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "while,for,switchの中ではありません");
        }
    }
    partial class Continue
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            n++;
            Obj val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            if (val2 != null) val2 = val2.GetterC(local);
            var count = 0;
            for (var i = local.blocks.Count - 1; i >= 0; i--)
            {
                var block = local.blocks[i];
                if (block.obj != null)
                {
                    if ((block.obj.obj.type == ObjType.IfBlock || block.obj.obj.type == ObjType.Switch) || block.obj.n == 0)
                    {
                        count++;
                    }
                    if (block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For)
                    {
                        Type ret = null;
                        if (block.obj.obj.type == ObjType.While) ret = (block.obj.obj as While).ret;
                        else if (block.obj.obj.type == ObjType.For) ret = (block.obj.obj as For).ret;

                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var rnpv = new Vari("i32*", "%ptr" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v" + LLVM.n++);
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", count.ToString()));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        Lab entry = block.obj.obj.entry;
                        if (block.obj.obj.type == ObjType.IfBlock)
                        {
                            end = (block.obj.obj as IfBlock).iflabel.entry;
                        }
                        var lab = local.labs.Last();
                        if (val2 == null)
                        {
                            val2 = new VoiVal();
                            local.llvm.func.comps.Add(new Br(null, entry));
                        }
                        if (val2.type == ObjType.Value)
                        {
                            var value = val2 as Value;
                            if (!block.obj.obj.ifv.ContainsKey(lab.name))
                            {
                                if (block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For)
                                {
                                    var v = val2.vari;
                                    if (value.cls == ret) block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                    else if (ret == local.Int)
                                    {
                                        v = new Vari("i32", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                        block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                    }
                                    else if (ret == local.Short)
                                    {
                                        v = new Vari("i16", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                        block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                    }
                                    else if (ret == local.Bool)
                                    {
                                        v = new Vari("i1", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                        block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                    }
                                    else block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    var varih = new Vari("%AddHashType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(varih, new Vari("%AddHashType*", "@AddHash")));
                                    local.llvm.func.comps.Add(new Call(null, varih, thgc4, new Vari("%StringType*", "null"), block.obj.obj.forvari, v));
                                }
                                else if (value.cls == ret) block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                else if (ret == local.Int)
                                {
                                    var v = new Vari("i32", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                }
                                else if (ret == local.Short)
                                {
                                    var v = new Vari("i16", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                }
                                else if (ret == local.Bool)
                                {
                                    var v = new Vari("i1", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Bitcast(v, val2.vari));
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), v);
                                }
                                else
                                {
                                    block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                                }
                            }
                            local.llvm.func.comps.Add(new Br(null, entry));
                        }
                        var lastblock = local.blocks.Last();
                        if (!lastblock.obj.obj.ifv.ContainsKey(lab.name)) lastblock.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), null);
                        n--;
                        return this;
                    }
                    else if (block.obj.obj.type == ObjType.Function || block.obj.obj.type == ObjType.ClassObj || block.obj.obj.type == ObjType.ModelObj || block.obj.obj.type == ObjType.GeneObj || block.obj.obj.type == ObjType.GenericFunction)
                    {
                        return Obj.Error(ObjType.Error, letter, "while,forの中ではありません");
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "while,forの中ではありません");
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            if (val2 != null) val2 = val2.GetterC(local);
            var count = 0;
            for (var i = local.blocks.Count - 1; i >= 0; i--)
            {
                var block = local.blocks[i];
                if (block.obj != null)
                {
                    if ((block.obj.obj.type == ObjType.IfBlock || block.obj.obj.type == ObjType.Switch) || block.obj.n == 0)
                    {
                        count++;
                    }
                    if (block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For)
                    {

                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var rnpv = new Vari("i32*", "%ptr" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v" + LLVM.n++);
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", count.ToString()));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        Lab entry = block.obj.obj.entry;
                        if (block.obj.obj.type == ObjType.IfBlock)
                        {
                            end = (block.obj.obj as IfBlock).iflabel.entry;
                        }
                        var lab = local.labs.Last();
                        if (val2 == null)
                        {
                            val2 = new VoiVal();
                            local.llvm.func.comps.Add(new Br(null, entry));
                        }
                        if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value || val2.type == ObjType.Block)
                        {
                            value = val2;
                            if (!block.obj.obj.ifv.ContainsKey(lab.name)) block.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), val2.vari);
                            local.llvm.func.comps.Add(new Br(null, entry));
                        }
                        var lastblock = local.blocks.Last();
                        if (!lastblock.obj.obj.ifv.ContainsKey(lab.name)) lastblock.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), null);
                        return this;
                    }
                    else if (block.obj.obj.type == ObjType.Function || block.obj.obj.type == ObjType.ClassObj || block.obj.obj.type == ObjType.ModelObj || block.obj.obj.type == ObjType.GeneObj || block.obj.obj.type == ObjType.GenericFunction)
                    {
                        return Obj.Error(ObjType.Error, letter, "while,forの中ではありません");
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "while,forの中ではありません");
        }
    }
    partial class Return
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            n++;
            Obj val2 = null;
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            if (val2 != null) val2 = val2.GetterC(local);
            var count = 1;
            for (var i = local.blocks.Count - 1; i >= 0; i--)
            {
                var block = local.blocks[i];
                if (block.obj != null)
                {
                    if ((block.obj.obj.type == ObjType.IfBlock || block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For || block.obj.obj.type == ObjType.Switch) || block.obj.n == 0)
                    {
                        count++;
                    }
                    if (block.obj.obj.type == ObjType.Function || block.obj.obj.type == ObjType.GenericFunction || block.obj.obj.type == ObjType.ServerFunction || block.obj.obj.type == ObjType.SignalFunction)
                    {
                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var rnpv = new Vari("i32*", "%ptr" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v" + LLVM.n++);
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", count.ToString()));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        n--;
                        if (local.llvm.func.async)
                        {
                            //set retvalue; final suspend; return;
                        }
                        if (local.llvm.func.async)
                        {
                            var obj = new Vari("%CoroFrameType*", "%obj");
                            if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value || val2.type == ObjType.Block)
                            {
                                var retp = new Vari("i8*", "%ret" + LLVM.n++);
                                local.llvm.comps.Add(new Gete("%CoroFrameType", retp, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                var vari = val2.vari;
                                if (val2.type == ObjType.Number || val2.type == ObjType.BoolVal)
                                {
                                    vari = new Vari("i8*", "%v" + LLVM.n++);
                                    local.llvm.comps.Add(new IntToPtr(vari, val2.vari));
                                }
                                local.llvm.comps.Add(new Store(retp, vari));
                            }
                            else if (val2.type == ObjType.Comment)
                            {
                                var clf = new Vari("%CreateListType", "%clf" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(clf, new Vari("%CreateListType*", "@CreateList")));
                                var list = new Vari("%ListType*", "%list" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(list, clf, new Vari("%ThreadGCType*", "%thgc"), new Vari("i32", "8"), new Vari("i1", "true")));
                                var v = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                                if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(v, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                                else local.llvm.func.comps.Add(new Load(v, local.blocks.Last().obj.obj.bas));
                                local.llvm.func.comps.Add(new Call(null, val2.vari, new Vari("%RootNodeType*", "%rn"), v, new Vari("%ElementType*", "null"), new Vari("%TreeElementType*", "null"), list));
                            }
                            else throw new Exception();
                            var stateval = new Vari("i32*", "%state" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                            var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", sfinp, obj, new Vari("i32", "0"), new Vari("i32", "10")));
                            var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(sfinv, sfinp));
                            local.llvm.func.comps.Add(new Store(stateval, sfinv));
                            var queueptr = new Vari("%CoroutineQueueType**", "%queueptr" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                            var queue = new Vari("%CoroutineQueueType*", "%queue" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(queue, queueptr));
                            var pushqueue = new Vari("%PushQueueType*", "%pushqueue" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(pushqueue, new Vari("%PushQueueType*", "@PushQueue")));
                            local.llvm.func.comps.Add(new Call(null, pushqueue, queue, obj));
                            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                        }
                        if (val2 == null)
                        {
                            local.llvm.func.comps.Add(new Ret(new Vari("void", "null")));
                            var lab = local.labs.Last();
                            var lastblock = local.blocks.Last();
                            if (!lastblock.obj.obj.ifv.ContainsKey(lab.name)) lastblock.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), null);
                            val2 = new VoiVal();
                            return new Value(local.Void);
                        }
                        if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value || val2.type == ObjType.Block)
                        {
                            local.llvm.func.comps.Add(new Ret(val2.vari));
                            var lab = local.labs.Last();
                            var lastblock = local.blocks.Last();
                            if (!lastblock.obj.obj.ifv.ContainsKey(lab.name)) lastblock.obj.obj.ifv[lab.name] = new IfValue(local.labs.Last(), null);
                            this.vari = val2.vari;
                            return new Value((block.obj.obj as Function).ret, val2.vari);
                        }
                        else if (val2.type == ObjType.Comment)
                        {
                            var clf = new Vari("%CreateListType", "%clf" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(clf, new Vari("%CreateListType*", "@CreateList")));
                            var list = new Vari("%ListType*", "%list" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(list, clf, new Vari("%ThreadGCType*", "%thgc"), new Vari("i32", "8"), new Vari("i1", "true")));
                            var v = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                            if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(v, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                            else local.llvm.func.comps.Add(new Load(v, local.blocks.Last().obj.obj.bas));
                            local.llvm.func.comps.Add(new Call(null, val2.vari, new Vari("%RootNodeType*", "%rn"), v, new Vari("%ElementType*", "null"), new Vari("%TreeElementType*", "null"), list));
                            return val2;
                        }
                        else throw new Exception("");
                    }
                    else if (block.obj.obj.type == ObjType.ClassObj)
                    {
                        return Obj.Error(ObjType.Error, val2.letter, "関数の中ではありません");
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "return文は関数の中でしか使えません");
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            if (val2 != null) val2 = val2.GetterC(local);
            var count = 1;
            for (var i = local.blocks.Count - 1; i >= 0; i--)
            {
                var block = local.blocks[i];
                if (block.obj != null)
                {
                    if ((block.obj.obj.type == ObjType.IfBlock || block.obj.obj.type == ObjType.While || block.obj.obj.type == ObjType.For || block.obj.obj.type == ObjType.Switch) || block.obj.n == 0)
                    {
                        count++;
                    }
                    if (block.obj.obj.type == ObjType.Function || block.obj.obj.type == ObjType.GenericFunction || block.obj.obj.type == ObjType.ServerFunction || block.obj.obj.type == ObjType.SignalFunction)
                    {
                        var rn4 = new Vari("%RootNodeType*", "%rn");
                        var rnpv = new Vari("i32*", "%ptr" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                        var rnpv12 = new Vari("i32", "%v" + LLVM.n++);
                        var rnp12load = new Load(rnpv12, rnpv);
                        local.llvm.func.comps.Add(rnp12load);
                        var rnpv2 = new Vari("i32", "%dec" + LLVM.n++);
                        var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", count.ToString()));
                        local.llvm.func.comps.Add(rnpsub);
                        local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                        if (val2 == null)
                        {
                            local.llvm.func.comps.Add(new Ret(new Vari("void", "null")));
                            val2 = new VoiVal();
                            return new Value(local.Void);
                        }
                        if (val2.type == ObjType.Number || val2.type == ObjType.StrObj || val2.type == ObjType.BoolVal || val2.type == ObjType.Value || val2.type == ObjType.Block)
                        {
                            local.llvm.func.comps.Add(new Ret(val2.vari));
                            this.vari = val2.vari;
                            return new Value((block.obj.obj as Function).ret, val2.vari);
                        }
                        else throw new Exception("");
                    }
                    else if (block.obj.obj.type == ObjType.ClassObj)
                    {
                        return Obj.Error(ObjType.Error, val2.letter, "関数の中ではありません");
                    }
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "return文は関数の中でしか使えません");
        }
    }
    partial class Goto
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.letter == local.letter && local.kouhos == null)
            {
                local.KouhoSetLabel();
            }
            if (val2.type == ObjType.Number)
            {
                value = (val2 as Number).value.ToString();
                return this;
            }
            else if (val2.type == ObjType.StrObj)
            {
                value = (val2 as StrObj).value;
                return this;
            }
            else if (val2.type == ObjType.Word)
            {
                value = (val2 as Word).name;
                return this;
            }
            return Obj.Error(ObjType.Error, val2.letter, "goto文の行き先を指定してください。");
        }
    }
    partial class Print
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                var val = val2.exeC(local).GetterC(local);
                var block = val as Block;
                local.llvm.func.comps.Add(new Call(null, new Vari("i32", "@putchar"), new Vari("i32", "40")));
                for (var i = 0; i < block.rets.Count; i++)
                {
                    if (i != 0) local.llvm.func.comps.Add(new Call(null, new Vari("i32", "@putchar"), new Vari("i32", "44")));
                    if (block.rets[i].type == ObjType.Value)
                    {
                        var value = block.rets[i] as Value;
                        if (value.cls == local.Int)
                        {
                            var v0 = new Vari("%NumberStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(v0, new Vari("%NumberStringType*", "@NumberString")));
                            var v05 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v05, v0, new Vari("%ThreadGCType*", "%thgc"), value.vari));
                            var v = new Vari("%PrintStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(v, new Vari("%PrintStringType*", "@PrintString")));
                            local.llvm.func.comps.Add(new Call(null, v, v05));
                        }
                        else if (value.cls == local.Short)
                        {
                            var v3 = new Vari("i32", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Zext(v3, value.vari));
                            var v0 = new Vari("%NumberStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(v0, new Vari("%NumberStringType*", "@NumberString")));
                            var v05 = new Vari("%StringType*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(v05, v0, new Vari("%ThreadGCType*", "%thgc"), v3));
                            var v = new Vari("%PrintStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(v, new Vari("%PrintStringType*", "@PrintString")));
                            local.llvm.func.comps.Add(new Call(null, v, v05));
                        }
                        else if (value.cls == local.Bool)
                        {
                        }
                        else if (value.cls == local.Str)
                        {
                            var v = new Vari("%PrintStringType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(v, new Vari("%PrintStringType*", "@PrintString")));
                            local.llvm.func.comps.Add(new Call(null, v, value.vari));
                        }
                        else if(value.cls.type == ObjType.ArrayType)
                        {
                            var n2 = 0;
                            var depth = 0;
                            Type cls = value.cls;
                        head:
                            var arrtype = cls as ArrType;
                            if (arrtype.cls == local.Int || arrtype.cls == local.Short)
                            {
                                n2 = 1;
                            }
                            else if (arrtype.cls == local.Str)
                            {
                                n2 = 0;
                            }
                            else if (arrtype.cls.type == ObjType.ArrayType)
                            {
                                depth++;
                                cls = arrtype.cls;
                                goto head;
                            }
                            else throw new Exception();
                            var pav = new Vari("%PrintArrayType", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(pav, new Vari("%PrintArrayType*", "@PrintArray")));
                            local.llvm.func.comps.Add(new Call(null, pav, value.vari, new Vari("i32", depth.ToString()), new Vari("i32", n2.ToString())));
                        }
                    }
                }
                local.llvm.func.comps.Add(new Call(null, new Vari("i32", "@putchar"), new Vari("i32", "41")));
                local.llvm.func.comps.Add(new Call(null, new Vari("i32", "@putchar"), new Vari("i32", "10")));
                n++;
                return new VoiVal();
            }
            val2 = null;
            for (; n < primary.children.Count - 1;)
            {
                Primary.NextC(primary, local, ref n, ref val2);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
            }
            val2 = val2.GetterC(local);
            if (val2.type == ObjType.Value)
            {
                n++;
                var value = val2 as Value;
                if (value.cls == local.Int)
                {
                    var v0 = new Vari("%NumberStringType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(v0, new Vari("%NumberStringType*", "@NumberString")));
                    var v05 = new Vari("%StringType*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Call(v05, v0, new Vari("%ThreadGCType*", "%thgc"), value.vari));
                    var v = new Vari("%PrintStringType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(v, new Vari("%PrintStringType*", "@PrintString")));
                    local.llvm.func.comps.Add(new Call(null, v, v05));
                }
                else if (value.cls == local.Short)
                {
                    var v3 = new Vari("i32", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Zext(v3, value.vari));
                    var v0 = new Vari("%NumberStringType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(v0, new Vari("%NumberStringType*", "@NumberString")));
                    var v05 = new Vari("%StringType*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Call(v05, v0, new Vari("%ThreadGCType*", "%thgc"), v3));
                    var v = new Vari("%PrintStringType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(v, new Vari("%PrintStringType*", "@PrintString")));
                    local.llvm.func.comps.Add(new Call(null, v, v05));
                }
                else if (value.cls == local.Bool)
                {
                }
                else if (value.cls == local.Str)
                {
                    var v = new Vari("%PrintStringType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(v, new Vari("%PrintStringType*", "@PrintString")));
                    local.llvm.func.comps.Add(new Call(null, v, val2.vari));
                }
                else if (value.cls.type == ObjType.ArrayType)
                {
                    var n2 = 0;
                    var depth = 0;
                    Type cls = value.cls;
                head:
                    var arrtype = cls as ArrType;
                    if (arrtype.cls == local.Int || arrtype.cls == local.Short)
                    {
                        n2 = 1;
                    }
                    else if (arrtype.cls == local.Str)
                    {
                        n2 = 0;
                    }
                    else if (arrtype.cls.type == ObjType.ArrayType)
                    {
                        depth++;
                        cls = arrtype.cls;
                        goto head;
                    }
                    else throw new Exception();
                    var pav = new Vari("%PrintArrayType", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(pav, new Vari("%PrintArrayType*", "@PrintArray")));
                    local.llvm.func.comps.Add(new Call(null, pav, value.vari, new Vari("i32", depth.ToString()), new Vari("i32", n2.ToString())));
                }
                return new VoiVal();
            }
            return Obj.Error(ObjType.Error, val2.letter, "()でprintの引数を指定してください");
        }
    }
    partial class Iterator
    {
        public override Obj GetterC(Local local)
        {
            if (m == -1) return value.rets[n];
            else return (value.rets[n] as Block).rets[m];
        }
    }
    partial class GenericFunction
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Block)
            {
                local.calls.Add(local.KouhoSetType);
                var val = val2.exeC(local).GetterC(local);
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var blk = val as Block;
                if (blk.rets.Count != vmap.Count) return Obj.Error(ObjType.Error, val2.let, "引数の数があっていません。");
                for (var i = 0; i < blk.rets.Count; i++)
                {
                    if (blk.rets[i] as Type == null) return Obj.Error(ObjType.Error, blk.letters[i], "ジェネリック関数には型を引数にしてください");
                }
                n++;
                val2 = primary.children[n];
                return new Function(ret) { blocks = blocks, draw = draw, bracket = bracket, block = blk };
            }
            return Obj.Error(ObjType.NG, val2.letter, "[]でジェリック関数の引数を指定してください");
        }
    }
    partial class Model
    {
        public int identity = 0;
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Block)
            {
                var blk0 = val2.exeC(local).GetterC(local) as Block;
                var val = blk0.rets[0];
                if (val is not ModelObj) return Obj.Error(ObjType.Error, val.letter, "ModelクラスかGeneクラスを指定してください"); ;
                var modelobj = val as ModelObj;
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Dot) {
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.Word)
                    {
                        var word2 = val2 as Word;
                        n++;
                        val2 = primary.children[n];
                        Obj where = null;
                    head:
                        if (word2.name == "Where")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val = val2.exeC(local).GetterC(local);
                                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                                var blk = val as Block;
                                if (blk.rets[0].type == ObjType.SqlString)
                                {
                                    where = blk.rets[0];
                                    goto head;
                                }
                                else if (blk.rets[0].type == ObjType.Value)
                                {
                                    var value = blk.rets[0] as Value;
                                    if (value.cls == local.Str)
                                    {
                                        where = blk.rets[0];
                                        goto head;
                                    }
                                    else return Obj.Error(ObjType.Error, val2.letter, "Whereの引数はString型かSqlString型です");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Whereの引数はString型かSqlString型です");
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "Whereの()が指定されていません");
                        }
                        else if (word2.name == "Sort")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val = val2.exeC(local).GetterC(local);
                                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                                var blk = val as Block;
                                if (blk.rets.Count != 1) return Obj.Error(ObjType.Error, val2.letter, "Sortの引数は1です");
                                else if (blk.rets[0].type != ObjType.Function) return Obj.Error(ObjType.Error, val2.letter, "Sortの引数は関数です");
                                var functype = new FuncType(local.Bool) { nochange = true };
                                functype.draws.Add(modelobj);
                                functype.draws.Add(modelobj);
                                var ret = TypeCheck.CheckCVB(functype, blk.rets[0], CheckType.Setter, local);
                                if (ret.type == ObjType.Wait || ret.type == ObjType.Error || ret.type == ObjType.NG) return ret;
                                var value = Value.New(modelobj, local, word2.letter);
                                if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                                return new Block(ObjType.Array) { rets = new List<Obj> { value }, letter = word2.letter }.Sort(blk.rets[0] as Function, local);
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "Sortの()が指定されていません");
                        }
                        else if (word2.name == "Update")
                        {
                            if (val2.type == ObjType.Dot)
                            {
                                n++;
                                val2 = primary.children[n];
                                if (val2.type == ObjType.Word)
                                {
                                    n++;
                                    val2 = primary.children[n];
                                    var word3 = val2 as Word;
                                    if (word3.name == "await")
                                    {
                                        n++;
                                        val2 = primary.children[n];
                                        if (val2.type == ObjType.Bracket)
                                        {
                                            n++; val = where;
                                            String sql = "";
                                            var ps = new Vari("ptr", "null");
                                            int pcount = 0;
                                            Vari v = null;
                                            var blk = val2.exeC(local) as Block;
                                            Vari v0 = null;
                                            SqlString sqs0 = null;
                                            if (blk.rets.Count >= 1)
                                            {
                                                if (blk.rets[0].type == ObjType.SqlString)
                                                {
                                                    sqs0 = blk.rets[0] as SqlString;
                                                    sqs0.value = "update " + modelobj.TableName() + " set " + sqs0.value;
                                                    for (var i = 1; i < blk.rets.Count; i++)
                                                    {
                                                        var sqs = blk.rets[i] as SqlString;
                                                        sql += ", ";
                                                        sqs0.value += sqs.value;
                                                        foreach (var va in sqs.varis)
                                                        {
                                                            if (!sqs0.varis.ContainsKey(va.Key))
                                                            {
                                                                sqs0.varis.Add(va.Key, va.Value);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (blk.rets[0].type == ObjType.Value)
                                                {
                                                    var value = val as Value;
                                                    ps = new Vari("ptr", "null");
                                                    if (value.cls == local.Str)
                                                    {
                                                        sql = "update " + modelobj.TableName() + " set ";
                                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                        local.llvm.strs.Add(sv);
                                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value.vari));
                                                        //+ limit 1;
                                                        v0 = strv;
                                                    }
                                                    else return Obj.Error(ObjType.Error, value.letter, "abc");
                                                }
                                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                            }
                                            else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                            var blk1 = modelobj.draw.children[0] as Block;
                                            if (where == null)
                                            {
                                                if (sqs0 == null)
                                                {
                                                    v = v0;
                                                }
                                                else
                                                {
                                                    sqs0.value += ";";
                                                    var sqs = sqs0;
                                                    sqs.Format();
                                                    sql = sqs0.value;
                                                    pcount = sqs.varis.Count;
                                                    ps = new Vari("ptr", "%ps");
                                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                    local.llvm.strs.Add(sv);
                                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                                    foreach (var kv in sqs.varis)
                                                    {
                                                        if (kv.Value.type == ObjType.Value)
                                                        {
                                                            var value = kv.Value as Value;
                                                            if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                            if (value.cls.type == ObjType.ModelObj)
                                                            {
                                                                if (value.cls == local.Int)
                                                                {
                                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                                    var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                    local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                                }
                                                                else if (value.cls == local.Str)
                                                                {
                                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                                    var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                    var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                                    //StringToUTF8
                                                                    var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                                    local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                                    var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Alloca(uc));
                                                                    var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                                    local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                                    var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                                    var uv = new Vari("i32", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Load(uv, uc));
                                                                    local.llvm.func.comps.Add(new Store(lptr, uv));
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (val.type == ObjType.SqlString)
                                            {
                                                if (sqs0 == null)
                                                {
                                                    var sqs = val as SqlString;
                                                    sqs.Format();
                                                    sql = " where " + sqs.value + ";";
                                                    pcount = sqs.varis.Count;
                                                    ps = new Vari("ptr", "%ps");
                                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                    local.llvm.strs.Add(sv);
                                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                                    foreach (var kv in sqs.varis)
                                                    {
                                                        if (kv.Value.type == ObjType.Value)
                                                        {
                                                            var value = kv.Value as Value;
                                                            if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                            if (value.cls.type == ObjType.ModelObj)
                                                            {
                                                                if (value.cls == local.Int)
                                                                {
                                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                                    var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                    local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                                }
                                                                else if (value.cls == local.Str)
                                                                {
                                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                                    var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                    var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                                    //StringToUTF8
                                                                    var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                                    local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                                    var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Alloca(uc));
                                                                    var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                                    local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                                    var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                                    var uv = new Vari("i32", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Load(uv, uc));
                                                                    local.llvm.func.comps.Add(new Store(lptr, uv));
                                                                }
                                                            }
                                                        }
                                                    }
                                                    var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                                    var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v0, v));
                                                    v = strv;
                                                }
                                                else
                                                {
                                                    var sqs = val as SqlString;
                                                    sqs0.value += " where " + sqs.value + ";";
                                                    foreach (var va in sqs.varis)
                                                    {
                                                        if (!sqs0.varis.ContainsKey(va.Key))
                                                        {
                                                            sqs0.varis.Add(va.Key, va.Value);
                                                        }
                                                    }
                                                    sqs0.Format();
                                                    sqs = sqs0;
                                                    sql = sqs0.value;
                                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                    local.llvm.strs.Add(sv);
                                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                    foreach (var kv in sqs.varis)
                                                    {
                                                        if (kv.Value.type == ObjType.Value)
                                                        {
                                                            var value = kv.Value as Value;
                                                            if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                            if (value.cls.type == ObjType.ModelObj)
                                                            {
                                                                if (value.cls == local.Int)
                                                                {
                                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                                    var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                    local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                                }
                                                                else if (value.cls == local.Str)
                                                                {
                                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                                    var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                    var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                                    //StringToUTF8
                                                                    var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                                    local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                                    var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Alloca(uc));
                                                                    var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                                    local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                                    var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                                    var uv = new Vari("i32", "%v" + LLVM.n++);
                                                                    local.llvm.func.comps.Add(new Load(uv, uc));
                                                                    local.llvm.func.comps.Add(new Store(lptr, uv));
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (val.type == ObjType.Value)
                                            {
                                                var value2 = val as Value;
                                                ps = new Vari("ptr", "null");
                                                if (value2.cls == local.Str)
                                                {
                                                    if (sqs0 == null)
                                                    {
                                                        sql = " where ";
                                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                        local.llvm.strs.Add(sv);
                                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value2.vari));
                                                        //+ limit 1;

                                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                                        var strv2 = new Vari("%StringType*", "%str" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(strv2, asv, thgc4, v0, strv));
                                                        v = strv2;
                                                        var sqs = sqs0;
                                                        sqs.Format();
                                                        sql = sqs0.value;
                                                        pcount = sqs.varis.Count;
                                                        ps = new Vari("ptr", "%ps");
                                                        local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                                        v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                        sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                        local.llvm.strs.Add(sv);
                                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        var v4 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(v4, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                        local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                                        var strv3 = new Vari("%StringType*", "%str" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(strv3, asv, thgc4, v4, v));
                                                        v = strv3;
                                                        foreach (var kv in sqs.varis)
                                                        {
                                                            if (kv.Value.type == ObjType.Value)
                                                            {
                                                                var value = kv.Value as Value;
                                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                                if (value.cls.type == ObjType.ModelObj)
                                                                {
                                                                    if (value.cls == local.Int)
                                                                    {
                                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                                        var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                        local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                                    }
                                                                    else if (value.cls == local.Str)
                                                                    {
                                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                                        var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                        var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                                        //StringToUTF8
                                                                        var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                                        local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                                        var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Alloca(uc));
                                                                        var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                                        local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                                        var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                                        var uv = new Vari("i32", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Load(uv, uc));
                                                                        local.llvm.func.comps.Add(new Store(lptr, uv));
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {

                                                        sql = "where ";
                                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                        local.llvm.strs.Add(sv);
                                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value2.vari));
                                                        //+ limit 1;

                                                        var sqs = sqs0;
                                                        sql = sqs0.value;
                                                        v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                        sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                        local.llvm.strs.Add(sv);
                                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));

                                                        //+ limit 1;
                                                        foreach (var kv in sqs.varis)
                                                        {
                                                            if (kv.Value.type == ObjType.Value)
                                                            {
                                                                var value = kv.Value as Value;
                                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                                if (value.cls.type == ObjType.ModelObj)
                                                                {
                                                                    if (value.cls == local.Int)
                                                                    {
                                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                                        var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                        local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                                    }
                                                                    else if (value.cls == local.Str)
                                                                    {
                                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                                        var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                        var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                                        //StringToUTF8
                                                                        var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                                        local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                                        var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Alloca(uc));
                                                                        var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                                        local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                                        var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                                        var uv = new Vari("i32", "%v" + LLVM.n++);
                                                                        local.llvm.func.comps.Add(new Load(uv, uc));
                                                                        local.llvm.func.comps.Add(new Store(lptr, uv));
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        var strv3 = new Vari("%StringType*", "%str" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(strv3, asv, thgc4, v, strv));
                                                        v = strv3;
                                                    }
                                                }
                                                else return Obj.Error(ObjType.Error, value2.letter, "abc");
                                            }
                                            else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                            if (local.llvm.func.async)
                                            {
                                                var tx = new Vari("i8*", "%tx" + LLVM.n++);
                                                var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, new Vari("%RootNodeType*", "%rn"), new Vari("i32", "0"), new Vari("i32", "2")));
                                                local.llvm.func.comps.Add(new Load(tx, txp));
                                                for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                                                local.comps = new List<Component>();
                                                local.llvm.func.vari.type = "%FuncType**";
                                                var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                                                var load = new Load(fv, local.llvm.func.vari);
                                                local.llvm.func.comps.Add(load);
                                                var go_v = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                                var rn4 = new Vari("%RootNodeType*", "%rn");
                                                var go_call = new Call(go_v, new Vari(modelobj.model + "*", modelobj.drawcall), rn4, fv);
                                                local.llvm.func.comps.Add(go_call);
                                                modelobj.Select(local);
                                                //Select * from users where (where) limit 1;
                                                var esql = new Vari("%ExecSqlType", "%esql");
                                                local.llvm.func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                                var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                                local.llvm.func.comps.Add(new Alloca(coroptr));
                                                local.llvm.func.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), go_v));
                                                var hv = new Vari("%CoroFrameType*", "%corov");
                                                local.llvm.func.comps.Add(new Load(hv, coroptr));

                                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                                var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                                var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(qv2, qv));
                                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                                local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                var nvari = new Vari("i32", (LLVM.n++).ToString());
                                                local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                                local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                                var lab6 = new Lab("aw" + LLVM.n++);
                                                local.llvm.func.comps.Add(lab6);
                                                var gev0 = new Vari(modelobj.model + "**", "%gv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", gev0, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                                                var gev02 = new Vari(modelobj.model + "*", "%gv2" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(gev02, gev0));
                                                var objptr0 = new Vari(modelobj.model + "**", "%objptr" + LLVM.n++);
                                                var alloca0 = new Alloca(objptr0);
                                                local.llvm.func.comps.Add(alloca0);
                                                var objstore0 = new Store(objptr0, gev02);
                                                local.llvm.func.comps.Add(objstore0);
                                                local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                                var retp = new Vari("i8**", "%ret" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", retp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                                var ret = new Vari("i8*", "%retv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(ret, retp));

                                                return new VoiVal();
                                            }
                                            else
                                            {
                                                var obj = new Vari("%CoroFrameType*", "%frame");
                                                var func = new Func(local.llvm, new Vari("void", "@First" + LLVM.n++), obj);
                                                local.llvm.comps.Add(func);
                                                func.async = true;

                                                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                var rngv = new Vari("%RootNodeType**", "%rngv");
                                                func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                                var rn4 = new Vari("%RootNodeType*", "%rn");
                                                func.comps.Add(new Load(rn4, rngv));
                                                func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                                var geterv = new Vari("i32*", "%state");
                                                func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                                var statev = new Vari("i32", "%statev" + LLVM.n++);
                                                func.comps.Add(new Load(statev, geterv));
                                                var list = new List<IfValue>();
                                                var lab10 = new Lab("coro_tx");
                                                func.sc = new SwitchComp(statev, lab10, list);
                                                func.comps.Add(func.sc);

                                                list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                                func.comps.Add(lab10);
                                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                var qvp = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                var qv1 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                func.comps.Add(new Load(qv1, qvp));
                                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(queueptr2, qv1));
                                                var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                                var bt = new Vari("%BeginTransactionType", "%bt");
                                                var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                                func.comps.Add(new Alloca(alv));
                                                func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                                func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qv1));
                                                func.comps.Add(new Load(hvn, alv));
                                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(qp, qv1));
                                                func.comps.Add(new Call(null, pushqueue1, qv1, hvn));
                                                lab10 = new Lab("coro_start" + LLVM.n++);
                                                list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                                var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                                var sfinp2 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                                func.comps.Add(new Store(sfinp2, new Vari("i32", "-3")));
                                                func.comps.Add(new Ret(new Vari("void", null)));
                                                func.comps.Add(lab10);

                                                var txp = new Vari("i8**", "%tx" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", txp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                                var tx = new Vari("i8*", "%txv" + LLVM.n++);
                                                func.comps.Add(new Load(tx, txp));
                                                var txps = new Vari("i8**", "%txps" + LLVM.n++);
                                                func.comps.Add(new Gete("%RootNodeType", txps, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                func.comps.Add(new Store(txps, tx));
                                                var esql = new Vari("%ExecSqlType", "%esql");
                                                func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                                var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                                func.comps.Add(new Alloca(coroptr));
                                                var op = new Vari("%Triptr**", "%op" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", op, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                                var obv = new Vari("%Triptr*", "%ov" + LLVM.n++);
                                                func.comps.Add(new Load(obv, op));
                                                var vp = new Vari("%StringType**", "%vp" + LLVM.n++);
                                                func.comps.Add(new Gete("%Triptr", vp, obv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                var v2 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                func.comps.Add(new Load(v2, vp));
                                                var pps1 = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                                func.comps.Add(new Gete("%Triptr", pps1, obv, new Vari("i32", "0"), new Vari("i32", "1")));
                                                var ps2 = new Vari("ptr", "%ps" + LLVM.n++);
                                                func.comps.Add(new Load(ps2, pps1));
                                                var sqpp = new Vari("ptr", "%sqpp" + LLVM.n++);
                                                func.comps.Add(new Gete("%Triptr", sqpp, obv, new Vari("i32", "0"), new Vari("i32", "2")));
                                                var sqp = new Vari("ptr", "%sqp" + LLVM.n++);
                                                func.comps.Add(new Load(sqp, sqpp));
                                                func.comps.Add(new Call(null, esql, coroptr, tx, v2, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), sqp));
                                                var hv = new Vari("%CoroFrameType*", "%corov");
                                                func.comps.Add(new Load(hv, coroptr));

                                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                                var qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                var qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                func.comps.Add(new Load(qv2, qv));
                                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(queueptr2, qv2));
                                                func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", sfinp, obj, new Vari("i32", "0"), new Vari("i32", "10")));
                                                var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                                                func.comps.Add(new Load(sfinv, sfinp));
                                                func.comps.Add(new Store(statevalptr2, sfinv));

                                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                func.comps.Add(new Ret(new Vari("void", null)));

                                                lab10 = new Lab("coro_txfin");

                                                list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                                func.comps.Add(lab10);

                                                var rp = new Vari("i8**", "%rp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", rp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                                var rv = new Vari("i8*", "%rv" + LLVM.n++);
                                                func.comps.Add(new Load(rv, rp));
                                                var rp2 = new Vari("i8**", "%rp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", rp2, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                                func.comps.Add(new Store(rp2, rv));
                                                txp = new Vari("i8**", "%tx" + LLVM.n++);
                                                func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                tx = new Vari("i8*", "%txv" + LLVM.n++);
                                                func.comps.Add(new Load(tx, txp));
                                                var txcf = new Vari("%TxFinishType", "%txcf" + LLVM.n++);
                                                func.comps.Add(new Load(txcf, new Vari("%TxFinishType*", "@TxCommit")));
                                                var hvp = new Vari("%CoroFrameType**", "%hv" + LLVM.n++);
                                                func.comps.Add(new Alloca(hvp));
                                                func.comps.Add(new Call(null, txcf, hvp, tx));
                                                hv = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                                func.comps.Add(new Load(hv, hvp));
                                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                func.comps.Add(new Load(qv2, qv));
                                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(queueptr2, qv2));
                                                func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));

                                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                func.comps.Add(new Ret(new Vari("void", null)));

                                                lab10 = new Lab("coro_end");
                                                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                                func.comps.Add(lab10);
                                                var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                                func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                                func.comps.Add(new Ret(new Vari("void", null)));

                                                var ov = new Vari("ptr", "%oj" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Call(ov, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                                                var sp = new Vari("%StringType**", "%sp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%Triptr", sp, ov, new Vari("i32", "0"), new Vari("i32", "0")));
                                                local.llvm.func.comps.Add(new Store(sp, v));
                                                var pps = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%Triptr", pps, ov, new Vari("i32", "0"), new Vari("i32", "1")));
                                                local.llvm.func.comps.Add(new Store(pps, ps));
                                                hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                                var root = new Vari("%RootNodeType*", "%rn");
                                                var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                                local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                                                txp = new Vari("ptr", "%txp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                tx = new Vari("ptr", "%txv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(tx, txp));
                                                var eq = new Vari("i1", "%eq" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Eq(eq, tx, new Vari("ptr", "null")));
                                                var l1 = new Lab("%notx" + LLVM.n++);
                                                var l2 = new Lab("%throuh" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                                local.llvm.func.comps.Add(l1);
                                                var stateptr = new Vari("i32*", "%stp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                                local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                                local.llvm.func.comps.Add(new Br(null, l2));
                                                local.llvm.func.comps.Add(l2);
                                                var wh = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(wh, new Vari("%WaitHandleType*", "@WaitHandle")));
                                                var ret = new Vari("%RowSet*", "%ret" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Call(ret, wh, thgc, hv));

                                                return new VoiVal();
                                                //make Coroutine Queue;
                                                //push handle;
                                                //loop
                                            }
                                        }
                                        else return Obj.Error(ObjType.Error, val2.letter, "FirstAwaitの()が指定されていません");

                                    }
                                }
                            }
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val = where;
                                String sql = "";
                                var ps = new Vari("ptr", "null");
                                int pcount = 0;
                                Vari v = null;
                                var blk = val2.exeC(local) as Block;
                                Vari v0 = null;
                                SqlString sqs0 = null;
                                if (blk.rets.Count >= 1)
                                {
                                    if (blk.rets[0].type == ObjType.SqlString)
                                    {
                                        sqs0 = blk.rets[0] as SqlString;
                                        sqs0.value = "update " + modelobj.TableName() + " set " + sqs0.value;
                                        for(var i = 1; i < blk.rets.Count; i++)
                                        {
                                            var sqs = blk.rets[i] as SqlString;
                                            sql += ", ";
                                            sqs0.value += sqs.value;
                                            foreach (var va in sqs.varis)
                                            {
                                                if (!sqs0.varis.ContainsKey(va.Key))
                                                {
                                                    sqs0.varis.Add(va.Key, va.Value);
                                                }
                                            }
                                        }
                                    }
                                    else if (blk.rets[0].type == ObjType.Value)
                                    {
                                        var value = val as Value;
                                        ps = new Vari("ptr", "null");
                                        if (value.cls == local.Str)
                                        {
                                            sql = "update " + modelobj.TableName() + " set ";
                                            var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                            var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                            var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                            local.llvm.strs.Add(sv);
                                            local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                            v = new Vari("%StringType*", "%v" + LLVM.n++);
                                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                            local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                            var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                            var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value.vari));
                                            //+ limit 1;
                                            v0 = strv;
                                        }
                                        else return Obj.Error(ObjType.Error, value.letter, "abc");
                                    }
                                    else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                var blk1 = modelobj.draw.children[0] as Block;
                                if (where == null)
                                {
                                    if (sqs0 == null)
                                    {
                                        v = v0;
                                    }
                                    else
                                    {
                                        sqs0.value += ";";
                                        var sqs = sqs0;
                                        sqs.Format();
                                        sql = sqs0.value;
                                        pcount = sqs.varis.Count;
                                        ps = new Vari("ptr", "%ps");
                                        local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                        local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                        foreach (var kv in sqs.varis)
                                        {
                                            if (kv.Value.type == ObjType.Value)
                                            {
                                                var value = kv.Value as Value;
                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                if (value.cls.type == ObjType.ModelObj)
                                                {
                                                    if (value.cls == local.Int)
                                                    {
                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                        var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                        local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                    }
                                                    else if (value.cls == local.Str)
                                                    {
                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                        var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                        var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                        //StringToUTF8
                                                        var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                        local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                        var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Alloca(uc));
                                                        var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                        local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                        var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                        var uv = new Vari("i32", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(uv, uc));
                                                        local.llvm.func.comps.Add(new Store(lptr, uv));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (val.type == ObjType.SqlString)
                                {
                                    if (sqs0 == null)
                                    {
                                        var sqs = val as SqlString;
                                        sqs.Format();
                                        sql = " where " + sqs.value + ";";
                                        pcount = sqs.varis.Count;
                                        ps = new Vari("ptr", "%ps");
                                        local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                        local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                        foreach (var kv in sqs.varis)
                                        {
                                            if (kv.Value.type == ObjType.Value)
                                            {
                                                var value = kv.Value as Value;
                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                if (value.cls.type == ObjType.ModelObj)
                                                {
                                                    if (value.cls == local.Int)
                                                    {
                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                        var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                        local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                    }
                                                    else if (value.cls == local.Str)
                                                    {
                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                        var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                        var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                        //StringToUTF8
                                                        var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                        local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                        var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Alloca(uc));
                                                        var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                        local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                        var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                        var uv = new Vari("i32", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(uv, uc));
                                                        local.llvm.func.comps.Add(new Store(lptr, uv));
                                                    }
                                                }
                                            }
                                        }
                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v0, v));
                                        v = strv;
                                    }
                                    else
                                    {
                                        var sqs = val as SqlString;
                                        sqs0.value += " where " + sqs.value + ";";
                                        foreach (var va in sqs.varis)
                                        {
                                            if (!sqs0.varis.ContainsKey(va.Key))
                                            {
                                                sqs0.varis.Add(va.Key, va.Value);
                                            }
                                        }
                                        sqs0.Format();
                                        sqs = sqs0;
                                        sql = sqs0.value;
                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                        foreach (var kv in sqs.varis)
                                        {
                                            if (kv.Value.type == ObjType.Value)
                                            {
                                                var value = kv.Value as Value;
                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                if (value.cls.type == ObjType.ModelObj)
                                                {
                                                    if (value.cls == local.Int)
                                                    {
                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                        var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                        local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                    }
                                                    else if (value.cls == local.Str)
                                                    {
                                                        var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                        var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                        local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                        var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                        var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                        //StringToUTF8
                                                        var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                        local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                        var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Alloca(uc));
                                                        var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                        local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                        var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                        var uv = new Vari("i32", "%v" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Load(uv, uc));
                                                        local.llvm.func.comps.Add(new Store(lptr, uv));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (val.type == ObjType.Value)
                                {
                                    var value2 = val as Value;
                                    ps = new Vari("ptr", "null");
                                    if (value2.cls == local.Str)
                                    {
                                        if (sqs0 == null)
                                        {
                                            sql = " where ";
                                            var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                            var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                            var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                            local.llvm.strs.Add(sv);
                                            local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                            v = new Vari("%StringType*", "%v" + LLVM.n++);
                                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                            local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                            var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                            var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value2.vari));
                                            //+ limit 1;

                                            local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                            var strv2 = new Vari("%StringType*", "%str" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(strv2, asv, thgc4, v0, strv));
                                            v = strv2;
                                            var sqs = sqs0;
                                            sqs.Format();
                                            sql = sqs0.value;
                                            pcount = sqs.varis.Count;
                                            ps = new Vari("ptr", "%ps");
                                            local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                            v2 = new Vari("i8*", "%v" + LLVM.n++);
                                            sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                            local.llvm.strs.Add(sv);
                                            local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                            var v4 = new Vari("%StringType*", "%v" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(v4, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                            local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                            var strv3 = new Vari("%StringType*", "%str" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(strv3, asv, thgc4, v4, v));
                                            v = strv3;
                                            foreach (var kv in sqs.varis)
                                            {
                                                if (kv.Value.type == ObjType.Value)
                                                {
                                                    var value = kv.Value as Value;
                                                    if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                    if (value.cls.type == ObjType.ModelObj)
                                                    {
                                                        if (value.cls == local.Int)
                                                        {
                                                            var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                            var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                            local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                            var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                            local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                        }
                                                        else if (value.cls == local.Str)
                                                        {
                                                            var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                            var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                            local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                            var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                            var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                            //StringToUTF8
                                                            var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                            local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                            var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Alloca(uc));
                                                            var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                            local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                            var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                            var uv = new Vari("i32", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Load(uv, uc));
                                                            local.llvm.func.comps.Add(new Store(lptr, uv));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {

                                            sql = "where ";
                                            var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                            var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                            var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                            local.llvm.strs.Add(sv);
                                            local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                            v = new Vari("%StringType*", "%v" + LLVM.n++);
                                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                            local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                            var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                            var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value2.vari));
                                            //+ limit 1;

                                            var sqs = sqs0;
                                            sql = sqs0.value;
                                            v2 = new Vari("i8*", "%v" + LLVM.n++);
                                            sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                            local.llvm.strs.Add(sv);
                                            local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                            v = new Vari("%StringType*", "%v" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                            
                                            //+ limit 1;
                                            foreach (var kv in sqs.varis)
                                            {
                                                if (kv.Value.type == ObjType.Value)
                                                {
                                                    var value = kv.Value as Value;
                                                    if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                    if (value.cls.type == ObjType.ModelObj)
                                                    {
                                                        if (value.cls == local.Int)
                                                        {
                                                            var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                            var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                            local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                            var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                            local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                        }
                                                        else if (value.cls == local.Str)
                                                        {
                                                            var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                            var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                            local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                            var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                            var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                            //StringToUTF8
                                                            var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                            local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                            var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Alloca(uc));
                                                            var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                            local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                            var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                            var uv = new Vari("i32", "%v" + LLVM.n++);
                                                            local.llvm.func.comps.Add(new Load(uv, uc));
                                                            local.llvm.func.comps.Add(new Store(lptr, uv));
                                                        }
                                                    }
                                                }
                                            }
                                            var strv3 = new Vari("%StringType*", "%str" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(strv3, asv, thgc4, v, strv));
                                            v = strv3;
                                        }
                                    }
                                    else return Obj.Error(ObjType.Error, value2.letter, "abc");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                
                                {
                                    var obj = new Vari("%CoroFrameType*", "%frame");
                                    var func = new Func(local.llvm, new Vari("void", "@First" + LLVM.n++), obj);
                                    local.llvm.comps.Add(func);
                                    func.async = true;

                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    var rngv = new Vari("%RootNodeType**", "%rngv");
                                    func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                    var rn4 = new Vari("%RootNodeType*", "%rn");
                                    func.comps.Add(new Load(rn4, rngv));
                                    func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                    var geterv = new Vari("i32*", "%state");
                                    func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var statev = new Vari("i32", "%statev" + LLVM.n++);
                                    func.comps.Add(new Load(statev, geterv));
                                    var list = new List<IfValue>();
                                    var lab10 = new Lab("coro_tx");
                                    func.sc = new SwitchComp(statev, lab10, list);
                                    func.comps.Add(func.sc);

                                    list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                    func.comps.Add(lab10);
                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var qvp = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv1 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv1, qvp));
                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv1));
                                    var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    var bt = new Vari("%BeginTransactionType", "%bt");
                                    var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                    func.comps.Add(new Alloca(alv));
                                    func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                    func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qv1));
                                    func.comps.Add(new Load(hvn, alv));
                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(qp, qv1));
                                    func.comps.Add(new Call(null, pushqueue1, qv1, hvn));
                                    lab10 = new Lab("coro_start" + LLVM.n++);
                                    list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                    var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                    var sfinp2 = new Vari("i32*", "%sfin" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "-10")));
                                    func.comps.Add(new Store(sfinp2, new Vari("i32", "-3")));
                                    func.comps.Add(new Ret(new Vari("void", null)));
                                    func.comps.Add(lab10);

                                    var txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", txp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txps = new Vari("i8**", "%txps" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txps, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    func.comps.Add(new Store(txps, tx));
                                    var esql = new Vari("%ExecSqlType", "%esql");
                                    func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                    var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                    func.comps.Add(new Alloca(coroptr));
                                    var op = new Vari("%Triptr**", "%op" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", op, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                    var obv = new Vari("%Triptr*", "%ov" + LLVM.n++);
                                    func.comps.Add(new Load(obv, op));
                                    var vp = new Vari("%StringType**", "%vp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", vp, obv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var v2 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    func.comps.Add(new Load(v2, vp));
                                    var pps1 = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", pps1, obv, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var ps2 = new Vari("ptr", "%ps" + LLVM.n++);
                                    func.comps.Add(new Load(ps2, pps1));
                                    var sqpp = new Vari("ptr", "%sqpp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", sqpp, obv, new Vari("i32", "0"), new Vari("i32", "2")));
                                    var sqp = new Vari("ptr", "%sqp" + LLVM.n++);
                                    func.comps.Add(new Load(sqp, sqpp));
                                    func.comps.Add(new Call(null, esql, coroptr, tx, v2, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), sqp));
                                    var hv = new Vari("%CoroFrameType*", "%corov");
                                    func.comps.Add(new Load(hv, coroptr));

                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var thgc = new Vari("%ThreadGCType*", "%thgc");
                                    var qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                                    func.comps.Add(new Load(sfinv, sfinp));
                                    func.comps.Add(new Store(statevalptr2, sfinv));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_txfin");

                                    list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                    func.comps.Add(lab10);

                                    var rp = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var rv = new Vari("i8*", "%rv" + LLVM.n++);
                                    func.comps.Add(new Load(rv, rp));
                                    var rp2 = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp2, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                    func.comps.Add(new Store(rp2, rv));
                                    txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txcf = new Vari("%TxFinishType", "%txcf" + LLVM.n++);
                                    func.comps.Add(new Load(txcf, new Vari("%TxFinishType*", "@TxCommit")));
                                    var hvp = new Vari("%CoroFrameType**", "%hv" + LLVM.n++);
                                    func.comps.Add(new Alloca(hvp));
                                    func.comps.Add(new Call(null, txcf, hvp, tx));
                                    hv = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    func.comps.Add(new Load(hv, hvp));
                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                    qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_end");
                                    list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                    func.comps.Add(lab10);
                                    var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    var ov = new Vari("ptr", "%oj" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(ov, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                                    var sp = new Vari("%StringType**", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", sp, ov, new Vari("i32", "0"), new Vari("i32", "0")));
                                    local.llvm.func.comps.Add(new Store(sp, v));
                                    hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                    var root = new Vari("%RootNodeType*", "%rn");
                                    var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                    local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                    qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                    txp = new Vari("ptr", "%txp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RootNodeType", rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("ptr", "%txv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(tx, txp));
                                    var eq = new Vari("i1", "%eq" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(eq, tx, new Vari("ptr", "null")));
                                    var l1 = new Lab("eq" + LLVM.n++);
                                    var l2 = new Lab("th" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                    local.llvm.func.comps.Add(l1);
                                    var stateptr = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                    local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                    local.llvm.func.comps.Add(new Br(null, l2));
                                    local.llvm.func.comps.Add(l2);
                                    local.llvm.func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                    var sfinp3 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));

                                    return new VoiVal();
                                    //make Coroutine Queue;
                                    //push handle;
                                    //loop
                                }
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "FirstAwaitの()が指定されていません");
                        }
                        else if (word2.name == "Delete")
                        {
                            if (val2.type == ObjType.Dot)
                            {
                                n++;
                                val2 = primary.children[n];
                                if (val2.type == ObjType.Word)
                                {
                                    var word3 = val2 as Word;
                                    n++;
                                    val2 = primary.children[n];
                                    if (word3.name == "await")
                                    {
                                        if (val2.type == ObjType.Bracket)
                                        {
                                            n++;
                                            val = where;
                                            String sql = "";
                                            var ps = new Vari("ptr", "null");
                                            int pcount = 0;
                                            Vari v = null;
                                            if (where == null)
                                            {
                                                sql = "delete from " +  modelobj.TableName() + ";";
                                                var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                local.llvm.strs.Add(sv);
                                                local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                            }
                                            else if (val.type == ObjType.SqlString)
                                            {
                                                var sqs = val as SqlString;
                                                sqs.Format();
                                                sql = "delete from " + modelobj.TableName() + " where " + sqs.value + ";";
                                                pcount = sqs.varis.Count;
                                                ps = new Vari("ptr", "%ps");
                                                local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                                var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                local.llvm.strs.Add(sv);
                                                local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                                foreach (var kv in sqs.varis)
                                                {
                                                    if (kv.Value.type == ObjType.Value)
                                                    {
                                                        var value = kv.Value as Value;
                                                        if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                        if (value.cls.type == ObjType.ModelObj)
                                                        {
                                                            if (value.cls == local.Int)
                                                            {
                                                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                                var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                            }
                                                            else if (value.cls == local.Str)
                                                            {
                                                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                                local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                                var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                                var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                                //StringToUTF8
                                                                var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                                local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                                var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Alloca(uc));
                                                                var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                                local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                                var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                                var uv = new Vari("i32", "%v" + LLVM.n++);
                                                                local.llvm.func.comps.Add(new Load(uv, uc));
                                                                local.llvm.func.comps.Add(new Store(lptr, uv));
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (val.type == ObjType.Value)
                                            {
                                                var value = val as Value;
                                                ps = new Vari("ptr", "null");
                                                if (value.cls == local.Str)
                                                {
                                                    sql = "delete from " + modelobj.TableName() + " where ";
                                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                                    local.llvm.strs.Add(sv);
                                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                                    var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                                    var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value.vari));
                                                    //+ limit 1;
                                                    v = strv;
                                                }
                                                else return Obj.Error(ObjType.Error, value.letter, "abc");
                                            }
                                            else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                            if (local.llvm.func.async)
                                            {
                                                var tx = new Vari("i8*", "%tx" + LLVM.n++);
                                                var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, new Vari("%RootNodeType*", "%rn"), new Vari("i32", "0"), new Vari("i32", "2")));
                                                local.llvm.func.comps.Add(new Load(tx, txp));
                                                var esql = new Vari("%ExecSqlType", "%esql");
                                                local.llvm.func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                                var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                                local.llvm.func.comps.Add(new Alloca(coroptr));
                                                local.llvm.func.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "null"), new Vari("ptr", "null")));
                                                var hv = new Vari("%CoroFrameType*", "%corov");
                                                local.llvm.func.comps.Add(new Load(hv, coroptr));

                                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                                var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", qv, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(qv2, qv));
                                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(qv2, qv));
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                                var stateptr = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                                local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "0")));
                                                local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                var nvari = new Vari("i32", (LLVM.n++).ToString());
                                                local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                                local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                                var lab6 = new Lab("aw" + LLVM.n++);
                                                local.llvm.func.comps.Add(lab6);
                                                var gev0 = new Vari(modelobj.model + "**", "%gv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", gev0, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                                                var gev02 = new Vari(modelobj.model + "*", "%gv2" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(gev02, gev0));
                                                var objptr0 = new Vari(modelobj.model + "**", "%objptr" + LLVM.n++);
                                                var alloca0 = new Alloca(objptr0);
                                                local.llvm.func.comps.Add(alloca0);
                                                var objstore0 = new Store(objptr0, gev02);
                                                local.llvm.func.comps.Add(objstore0);
                                                local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                                var retp = new Vari("i8**", "%ret" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", retp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                                var ret = new Vari("i8*", "%retv" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(ret, retp));

                                                return new VoiVal();
                                            }
                                            else
                                            {
                                                var obj = new Vari("%CoroFrameType*", "%frame");
                                                var func = new Func(local.llvm, new Vari("void", "@First" + LLVM.n++), obj);
                                                local.llvm.comps.Add(func);
                                                func.async = true;

                                                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                                var rngv = new Vari("%RootNodeType**", "%rngv");
                                                func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                                var rn4 = new Vari("%RootNodeType*", "%rn");
                                                func.comps.Add(new Load(rn4, rngv));
                                                func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                                var geterv = new Vari("i32*", "%state");
                                                func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                                var statev = new Vari("i32", "%statev" + LLVM.n++);
                                                func.comps.Add(new Load(statev, geterv));
                                                var list = new List<IfValue>();
                                                var lab10 = new Lab("coro_tx");
                                                func.sc = new SwitchComp(statev, lab10, list);
                                                func.comps.Add(func.sc);

                                                list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                                func.comps.Add(lab10);
                                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                var qvp = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                var qv1 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                func.comps.Add(new Load(qv1, qvp));
                                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(queueptr2, qv1));
                                                var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                                var bt = new Vari("%BeginTransactionType", "%bt");
                                                var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                                func.comps.Add(new Alloca(alv));
                                                func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                                func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qv1));
                                                func.comps.Add(new Load(hvn, alv));
                                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(qp, qv1));
                                                func.comps.Add(new Call(null, pushqueue1, qv1, hvn));
                                                lab10 = new Lab("coro_start" + LLVM.n++);
                                                list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                                var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                                var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                                func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                                                func.comps.Add(new Ret(new Vari("void", null)));
                                                func.comps.Add(lab10);

                                                var txp = new Vari("i8**", "%tx" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", txp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                                var tx = new Vari("i8*", "%txv" + LLVM.n++);
                                                func.comps.Add(new Load(tx, txp));
                                                var txps = new Vari("i8**", "%txps" + LLVM.n++);
                                                func.comps.Add(new Gete("%RootNodeType", txps, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                func.comps.Add(new Store(txps, tx));
                                                var esql = new Vari("%ExecSqlType", "%esql");
                                                func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                                var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                                func.comps.Add(new Alloca(coroptr));
                                                var op = new Vari("%Triptr**", "%op" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", op, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                                var obv = new Vari("%Triptr*", "%ov" + LLVM.n++);
                                                func.comps.Add(new Load(obv, op));
                                                var vp = new Vari("%StringType**", "%vp" + LLVM.n++);
                                                func.comps.Add(new Gete("%Triptr", vp, obv, new Vari("i32", "0"), new Vari("i32", "0")));
                                                var v2 = new Vari("%StringType*", "%v" + LLVM.n++);
                                                func.comps.Add(new Load(v2, vp));
                                                var pps1 = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                                func.comps.Add(new Gete("%Triptr", pps1, obv, new Vari("i32", "0"), new Vari("i32", "1")));
                                                var ps2 = new Vari("ptr", "%ps" + LLVM.n++);
                                                func.comps.Add(new Load(ps2, pps1));
                                                var sqpp = new Vari("ptr", "%sqpp" + LLVM.n++);
                                                func.comps.Add(new Gete("%Triptr", sqpp, obv, new Vari("i32", "0"), new Vari("i32", "2")));
                                                var sqp = new Vari("ptr", "%sqp" + LLVM.n++);
                                                func.comps.Add(new Load(sqp, sqpp));
                                                func.comps.Add(new Call(null, esql, coroptr, tx, v2, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), sqp));
                                                var hv = new Vari("%CoroFrameType*", "%corov");
                                                func.comps.Add(new Load(hv, coroptr));

                                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                                var qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                var qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                func.comps.Add(new Load(qv2, qv));
                                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(queueptr2, qv2));
                                                func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                var sfinp2 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                                var sfinv2 = new Vari("i32", "%sfinv" + LLVM.n++);
                                                func.comps.Add(new Load(sfinv2, sfinp2));
                                                func.comps.Add(new Store(statevalptr2, sfinv2));

                                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                func.comps.Add(new Ret(new Vari("void", null)));

                                                lab10 = new Lab("coro_txfin");

                                                list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                                func.comps.Add(lab10);

                                                var rp = new Vari("i8**", "%rp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", rp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                                var rv = new Vari("i8*", "%rv" + LLVM.n++);
                                                func.comps.Add(new Load(rv, rp));
                                                var rp2 = new Vari("i8**", "%rp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", rp2, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                                func.comps.Add(new Store(rp2, rv));
                                                txp = new Vari("i8**", "%tx" + LLVM.n++);
                                                func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                tx = new Vari("i8*", "%txv" + LLVM.n++);
                                                func.comps.Add(new Load(tx, txp));
                                                var txcf = new Vari("%TxFinishType", "%txcf" + LLVM.n++);
                                                func.comps.Add(new Load(txcf, new Vari("%TxFinishType*", "@TxCommit")));
                                                var hvp = new Vari("%CoroFrameType**", "%hv" + LLVM.n++);
                                                func.comps.Add(new Alloca(hvp));
                                                func.comps.Add(new Call(null, txcf, hvp, tx));
                                                hv = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                                func.comps.Add(new Load(hv, hvp));
                                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                func.comps.Add(new Load(qv2, qv));
                                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                func.comps.Add(new Store(queueptr2, qv2));
                                                func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));

                                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                func.comps.Add(new Ret(new Vari("void", null)));

                                                lab10 = new Lab("coro_end");
                                                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                                func.comps.Add(lab10);
                                                var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                                func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                                func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                                func.comps.Add(new Ret(new Vari("void", null)));

                                                var ov = new Vari("ptr", "%oj" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Call(ov, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                                                var sp = new Vari("%StringType**", "%sp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%Triptr", sp, ov, new Vari("i32", "0"), new Vari("i32", "0")));
                                                local.llvm.func.comps.Add(new Store(sp, v));
                                                var pps = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%Triptr", pps, ov, new Vari("i32", "0"), new Vari("i32", "1")));
                                                local.llvm.func.comps.Add(new Store(pps, ps));
                                                hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                                var root = new Vari("%RootNodeType*", "%rn");
                                                var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                                local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                                                var wh = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                                var stateptr = new Vari("i32*", "%stp" + LLVM.n++);
                                                txp = new Vari("ptr", "%txp" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                tx = new Vari("ptr", "%tx" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Load(tx, txp));
                                                var eq = new Vari("i1", "%eq" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Eq(eq, tx, new Vari("ptr", "null")));
                                                var l1 = new Lab("eq" + LLVM.n++);
                                                var l2 = new Lab("th" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                                local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                                local.llvm.func.comps.Add(new Br(null, l2));
                                                local.llvm.func.comps.Add(l2);
                                                local.llvm.func.comps.Add(new Load(wh, new Vari("%WaitHandleType*", "@WaitHandle")));
                                                var ret = new Vari("%RowSet*", "%ret" + LLVM.n++);
                                                local.llvm.func.comps.Add(new Call(ret, wh, thgc, hv));

                                                return new VoiVal();
                                                //make Coroutine Queue;
                                                //push handle;
                                                //loop
                                            }
                                        }
                                        else return Obj.Error(ObjType.Error, val2.letter, "FirstAwaitの()が指定されていません");
                                    }
                                }
                            }
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val = where;
                                String sql = "";
                                var ps = new Vari("ptr", "null");
                                int pcount = 0;
                                Vari v = null;
                                if (where == null)
                                {
                                    sql = "delete from " + modelobj.TableName() + ";";
                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                    local.llvm.strs.Add(sv);
                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                }
                                else if (val.type == ObjType.SqlString)
                                {
                                    var sqs = val as SqlString;
                                    sqs.Format();
                                    sql = "delete from " + modelobj.TableName() + " where " + sqs.value + ";";
                                    pcount = sqs.varis.Count;
                                    ps = new Vari("ptr", "%ps");
                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                    local.llvm.strs.Add(sv);
                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                    foreach (var kv in sqs.varis)
                                    {
                                        if (kv.Value.type == ObjType.Value)
                                        {
                                            var value = kv.Value as Value;
                                            if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                            if (value.cls.type == ObjType.ModelObj)
                                            {
                                                if (value.cls == local.Int)
                                                {
                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                    var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                }
                                                else if (value.cls == local.Str)
                                                {
                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                    var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                    //StringToUTF8
                                                    var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                    local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                    var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Alloca(uc));
                                                    var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                    local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                    var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                    var uv = new Vari("i32", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(uv, uc));
                                                    local.llvm.func.comps.Add(new Store(lptr, uv));
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (val.type == ObjType.Value)
                                {
                                    var value = val as Value;
                                    ps = new Vari("ptr", "null");
                                    if (value.cls == local.Str)
                                    {
                                        sql = "delete from " + modelobj.TableName() + " where ";
                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value.vari));
                                        //+ limit 1;
                                        v = strv;
                                    }
                                    else return Obj.Error(ObjType.Error, value.letter, "abc");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                {
                                    var obj = new Vari("%CoroFrameType*", "%frame");
                                    var func = new Func(local.llvm, new Vari("void", "@First" + LLVM.n++), obj);
                                    local.llvm.comps.Add(func);
                                    func.async = true;

                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    var rngv = new Vari("%RootNodeType**", "%rngv");
                                    func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                    var rn4 = new Vari("%RootNodeType*", "%rn");
                                    func.comps.Add(new Load(rn4, rngv));
                                    func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                    var geterv = new Vari("i32*", "%state");
                                    func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var statev = new Vari("i32", "%statev" + LLVM.n++);
                                    func.comps.Add(new Load(statev, geterv));
                                    var list = new List<IfValue>();
                                    var lab10 = new Lab("coro_tx");
                                    func.sc = new SwitchComp(statev, lab10, list);
                                    func.comps.Add(func.sc);

                                    list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                    func.comps.Add(lab10);
                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var qvp = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv1 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv1, qvp));
                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv1));
                                    var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    var bt = new Vari("%BeginTransactionType", "%bt");
                                    var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                    func.comps.Add(new Alloca(alv));
                                    func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                    func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qv1));
                                    func.comps.Add(new Load(hvn, alv));
                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(qp, qv1));
                                    func.comps.Add(new Call(null, pushqueue1, qv1, hvn));
                                    lab10 = new Lab("coro_start" + LLVM.n++);
                                    list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                    var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                    var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                                    func.comps.Add(new Ret(new Vari("void", null)));
                                    func.comps.Add(lab10);

                                    var txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", txp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txps = new Vari("i8**", "%txps" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txps, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    func.comps.Add(new Store(txps, tx));
                                    var esql = new Vari("%ExecSqlType", "%esql");
                                    func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                    var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                    func.comps.Add(new Alloca(coroptr));
                                    var op = new Vari("%Triptr**", "%op" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", op, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                    var obv = new Vari("%Triptr*", "%ov" + LLVM.n++);
                                    func.comps.Add(new Load(obv, op));
                                    var vp = new Vari("%StringType**", "%vp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", vp, obv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var v2 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    func.comps.Add(new Load(v2, vp));
                                    var pps1 = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", pps1, obv, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var ps2 = new Vari("ptr", "%ps" + LLVM.n++);
                                    func.comps.Add(new Load(ps2, pps1));
                                    func.comps.Add(new Call(null, esql, coroptr, tx, v2, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "null"), new Vari("ptr", "null")));
                                    var hv = new Vari("%CoroFrameType*", "%corov");
                                    func.comps.Add(new Load(hv, coroptr));

                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var thgc = new Vari("%ThreadGCType*", "%thgc");
                                    var qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var sfinp2 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    var sfinv2 = new Vari("i32", "%sfinv" + LLVM.n++);
                                    func.comps.Add(new Load(sfinv2, sfinp2));
                                    func.comps.Add(new Store(statevalptr2, sfinv2));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_txfin");

                                    list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                    func.comps.Add(lab10);

                                    var rp = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var rv = new Vari("i8*", "%rv" + LLVM.n++);
                                    func.comps.Add(new Load(rv, rp));
                                    var rp2 = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp2, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                    func.comps.Add(new Store(rp2, rv));
                                    txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txcf = new Vari("%TxFinishType", "%txcf" + LLVM.n++);
                                    func.comps.Add(new Load(txcf, new Vari("%TxFinishType*", "@TxCommit")));
                                    var hvp = new Vari("%CoroFrameType**", "%hv" + LLVM.n++);
                                    func.comps.Add(new Alloca(hvp));
                                    func.comps.Add(new Call(null, txcf, hvp, tx));
                                    hv = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    func.comps.Add(new Load(hv, hvp));
                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_end");
                                    list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                    func.comps.Add(lab10);
                                    var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    var ov = new Vari("ptr", "%oj" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(ov, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                                    var sp = new Vari("%StringType**", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", sp, ov, new Vari("i32", "0"), new Vari("i32", "0")));
                                    local.llvm.func.comps.Add(new Store(sp, v));
                                    hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                    var root = new Vari("%RootNodeType*", "%rn");
                                    var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                    local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                    qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                    txp = new Vari("ptr", "%txp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("ptr", "%tx" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(tx, txp));
                                    var eq = new Vari("i1", "%eq" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(eq, tx, new Vari("ptr", "null")));
                                    var l1 = new Lab("eq" + LLVM.n++);
                                    var l2 = new Lab("th" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                    local.llvm.func.comps.Add(l1);
                                    var stateptr = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                    local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                    local.llvm.func.comps.Add(new Br(null, l2));
                                    local.llvm.func.comps.Add(l2);
                                    local.llvm.func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                    local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));

                                    return new VoiVal();
                                    //make Coroutine Queue;
                                    //push handle;
                                    //loop
                                }
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "FirstAwaitの()が指定されていません");
                        }
                        else if (word2.name == "FirstAwait")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val = where;
                                String sql = "";
                                var ps = new Vari("ptr", "null");
                                int pcount = 0;
                                Vari v = null;
                                var blk1 = modelobj.draw.children[0] as Block;
                                String vstr = "id";
                                foreach(var kv in blk1.vmapA)
                                {
                                    if (kv.Value.type == ObjType.Variable)
                                    {
                                        var variable = kv.Value as Variable;
                                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                        if (variable.cls.type == ObjType.ModelObj)
                                        {
                                            vstr += ", " + kv.Key;
                                        }
                                    }
                                }
                                var blk2 = modelobj.draw.children[1] as Block;
                                foreach(var kv in blk2.vmapA)
                                {
                                    if (kv.Value.type == ObjType.Variable)
                                    {
                                        if (kv.Key == "id") continue;
                                        var variable = kv.Value as Variable;
                                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                        if (variable.cls.type == ObjType.ModelObj)
                                        {
                                            vstr += ", " + kv.Key;
                                        }
                                    }
                                }
                                if (where == null)
                                {
                                    sql = "select " + vstr + " from " + modelobj.TableName() + " limit 1;";
                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                    local.llvm.strs.Add(sv);
                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                }
                                else if (val.type == ObjType.SqlString)
                                {
                                    var sqs = val as SqlString;
                                    sqs.Format();
                                    sql = "select " + vstr + " from " + modelobj.TableName() + " where " + sqs.value + " limit 1;";
                                    pcount = sqs.varis.Count;
                                    ps = new Vari("ptr", "%ps");
                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                    local.llvm.strs.Add(sv);
                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                    foreach (var kv in sqs.varis)
                                    {
                                        if (kv.Value.type == ObjType.Value)
                                        {
                                            var value = kv.Value as Value;
                                            if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                            if (value.cls.type == ObjType.ModelObj)
                                            {
                                                if (value.cls == local.Int)
                                                {
                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                    var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                }
                                                else if (value.cls == local.Str)
                                                {
                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                    var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                    //StringToUTF8
                                                    var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                    local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                    var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Alloca(uc));
                                                    var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                    local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                    var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                    var uv = new Vari("i32", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(uv, uc));
                                                    local.llvm.func.comps.Add(new Store(lptr, uv));
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (val.type == ObjType.Value)
                                {
                                    var value = val as Value;
                                    ps = new Vari("ptr", "null");
                                    if (value.cls == local.Str)
                                    {
                                        sql = "select " + vstr + " from " + modelobj.TableName() + " where ";
                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value.vari));
                                        //+ limit 1;
                                        var lim = " limit 1;";
                                        v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        sv = new StrV("@s" + LLVM.n++, lim, lim.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (lim.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", lim.Length.ToString()), new Vari("i32", 1.ToString())));
                                        var strv2 = new Vari("%StringType*", "%str" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(strv2, asv, thgc4, strv, v));
                                        v = strv2;
                                    }
                                    else return Obj.Error(ObjType.Error, value.letter, "abc");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                if (local.llvm.func.async)
                                {
                                    var tx = new Vari("i8*", "%tx" + LLVM.n++);
                                    var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, new Vari("%RootNodeType*", "%rn"), new Vari("i32", "0"), new Vari("i32", "2")));
                                    local.llvm.func.comps.Add(new Load(tx, txp));
                                    for(var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                                    local.comps = new List<Component>();
                                    local.llvm.func.vari.type = "%FuncType**";
                                    var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var load = new Load(fv, local.llvm.func.vari);
                                    local.llvm.func.comps.Add(load);
                                    var go_v = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var rn4 = new Vari("%RootNodeType*", "%rn");
                                    var go_call = new Call(go_v, new Vari(modelobj.model + "*", modelobj.drawcall), rn4, fv);
                                    local.llvm.func.comps.Add(go_call);
                                    modelobj.Select(local);
                                    //Select * from users where (where) limit 1;
                                    var esql = new Vari("%ExecSqlType", "%esql");
                                    local.llvm.func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                    var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                    local.llvm.func.comps.Add(new Alloca(coroptr));
                                    local.llvm.func.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), go_v));
                                    var hv = new Vari("%CoroFrameType*", "%corov");
                                    local.llvm.func.comps.Add(new Load(hv, coroptr));

                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var thgc = new Vari("%ThreadGCType*", "%thgc");
                                    var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(qv2, qv));
                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                    local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    var stateptr = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                    local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "0")));
                                    var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var nvari = new Vari("i32", (LLVM.n++).ToString());
                                    local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                    local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                    var lab6 = new Lab("aw" + LLVM.n++);
                                    local.llvm.func.comps.Add(lab6);
                                    var gev0 = new Vari(modelobj.model + "**", "%gv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", gev0, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                                    var gev02 = new Vari(modelobj.model + "*", "%gv2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(gev02, gev0));
                                    var objptr0 = new Vari(modelobj.model + "**", "%objptr" + LLVM.n++);
                                    var alloca0 = new Alloca(objptr0);
                                    local.llvm.func.comps.Add(alloca0);
                                    var objstore0 = new Store(objptr0, gev02);
                                    local.llvm.func.comps.Add(objstore0);
                                    local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                    var retp = new Vari("i8**", "%ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", retp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var ret = new Vari("i8*", "%retv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ret, retp));



                                    var rowsp = new Vari("%HashType**", "%rowsp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RowSet", rowsp, ret, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var rowsv = new Vari("%HashType*", "%rowsv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(rowsv, rowsp));
                                    var kvsp = new Vari("%ListType**", "%kvs" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%HashType", kvsp, rowsv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var kvs = new Vari("%ListType*", "%kvs" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(kvs, kvsp));
                                    var sizep = new Vari("i32*", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ListType", sizep, kvs, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var size = new Vari("i32", "%s" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(size, sizep));

                                    var equal = new Vari("i1", "%equal" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(equal, size, new Vari("i32", "0")));
                                    var lab1 = new Lab("coro_end" + LLVM.n++);
                                    var lab2 = new Lab("coro_ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(equal, lab1, lab2));
                                    local.llvm.func.comps.Add(lab2);


                                    var glf = new Vari("%GetListType", "%glf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(glf, new Vari("%GetListType*", "@GetList")));
                                    var op2 = new Vari("ptr", "%op" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(op2, glf, kvs, new Vari("i32", "0")));
                                    var ov2 = new Vari("%KeyValueType*", "%ov" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ov2, op2));
                                    var op3 = new Vari(modelobj.model + "**", "%op3" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%KeyValueType", op3, ov2, new Vari("i32", "0"), new Vari("i32", "2")));
                                    var ov3 = new Vari(modelobj.model + "*", "%ov3" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ov3, op3));


                                    local.llvm.func.comps.Add(new Br(null, lab1));
                                    local.llvm.func.comps.Add(lab1);
                                    var vv = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var dict = new Dictionary<String, IfValue>();
                                    dict.Add(lab2.name, new IfValue(lab2, ov3));
                                    dict.Add(lab6.name, new IfValue(lab6, new Vari(modelobj.model + "*", "null")));
                                    local.llvm.func.comps.Add(new Phi(vv, dict));
                                    return new Value(modelobj, vv);
                                }
                                else
                                {
                                    var obj = new Vari("%CoroFrameType*", "%frame");
                                    var func = new Func(local.llvm, new Vari("void", "@First" + LLVM.n++), obj);
                                    local.llvm.comps.Add(func);
                                    func.async = true;

                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    var rngv = new Vari("%RootNodeType**", "%rngv");
                                    func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                    var rn4 = new Vari("%RootNodeType*", "%rn");
                                    func.comps.Add(new Load(rn4, rngv));
                                    func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                    var geterv = new Vari("i32*", "%state");
                                    func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var statev = new Vari("i32", "%statev" + LLVM.n++);
                                    func.comps.Add(new Load(statev, geterv));
                                    var list = new List<IfValue>();
                                    var lab10 = new Lab("coro_tx");
                                    func.sc = new SwitchComp(statev, lab10, list);
                                    func.comps.Add(func.sc);

                                    list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                    func.comps.Add(lab10);
                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var qvp = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv1 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv1, qvp));
                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv1));
                                    var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    var bt = new Vari("%BeginTransactionType", "%bt");
                                    var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                    func.comps.Add(new Alloca(alv));
                                    func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                    func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qv1));
                                    func.comps.Add(new Load(hvn, alv));
                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(qp, qv1));
                                    func.comps.Add(new Call(null, pushqueue1, qv1, hvn));
                                    lab10 = new Lab("coro_start" + LLVM.n++);
                                    list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                    var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                    var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                                    func.comps.Add(new Ret(new Vari("void", null)));
                                    func.comps.Add(lab10);

                                    var txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", txp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txps = new Vari("i8**", "%txps" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txps, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    func.comps.Add(new Store(txps, tx));
                                    var esql = new Vari("%ExecSqlType", "%esql");
                                    func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                    var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                    func.comps.Add(new Alloca(coroptr));
                                    var op = new Vari("%Triptr**", "%op" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", op, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                    var obv = new Vari("%Triptr*", "%ov" + LLVM.n++);
                                    func.comps.Add(new Load(obv, op));
                                    var vp = new Vari("%StringType**", "%vp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", vp, obv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var v2 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    func.comps.Add(new Load(v2, vp));
                                    var pps1 = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", pps1, obv, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var ps2 = new Vari("ptr", "%ps" + LLVM.n++);
                                    func.comps.Add(new Load(ps2, pps1));
                                    var sqpp = new Vari("ptr", "%sqpp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", sqpp, obv, new Vari("i32", "0"), new Vari("i32", "2")));
                                    var sqp = new Vari("ptr", "%sqp" + LLVM.n++);
                                    func.comps.Add(new Load(sqp, sqpp));
                                    func.comps.Add(new Call(null, esql, coroptr, tx, v2, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), sqp));
                                    var hv = new Vari("%CoroFrameType*", "%corov");
                                    func.comps.Add(new Load(hv, coroptr));

                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var thgc = new Vari("%ThreadGCType*", "%thgc");
                                    var qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var sfinp2 = new Vari("i32*", "%sinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    var sfinv2 = new Vari("i32", "%sfinv" + LLVM.n++);
                                    func.comps.Add(new Load(sfinv2, sfinp2));
                                    func.comps.Add(new Store(statevalptr2, sfinv2));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_txfin");

                                    list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                    func.comps.Add(lab10);

                                    var rp = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var rv = new Vari("i8*", "%rv" + LLVM.n++);
                                    func.comps.Add(new Load(rv, rp));
                                    var rp2 = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp2, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                    func.comps.Add(new Store(rp2, rv));
                                    txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txcf = new Vari("%TxFinishType", "%txcf" + LLVM.n++);
                                    func.comps.Add(new Load(txcf, new Vari("%TxFinishType*", "@TxCommit")));
                                    var hvp = new Vari("%CoroFrameType**", "%hv" + LLVM.n++);
                                    func.comps.Add(new Alloca(hvp));
                                    func.comps.Add(new Call(null, txcf, hvp, tx));
                                    hv = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    func.comps.Add(new Load(hv, hvp));
                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_end");
                                    list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                    func.comps.Add(lab10);
                                    var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    var ov = new Vari("ptr", "%oj" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(ov, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                                    var sp = new Vari("%StringType**", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", sp, ov, new Vari("i32", "0"), new Vari("i32", "0")));
                                    local.llvm.func.comps.Add(new Store(sp, v));
                                    var pps = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", pps, ov, new Vari("i32", "0"), new Vari("i32", "1")));
                                    local.llvm.func.comps.Add(new Store(pps, ps));
                                    for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                                    local.comps = new List<Component>();
                                    local.llvm.func.vari.type = "%FuncType**";
                                    var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var load = new Load(fv, local.llvm.func.vari);
                                    local.llvm.func.comps.Add(load);
                                    var go_v = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var go_call = new Call(go_v, new Vari(modelobj.model + "*", modelobj.drawcall), rn4, fv);
                                    local.llvm.func.comps.Add(go_call);
                                    modelobj.Select(local);
                                    var pps2 = new Vari("%SqlParamType**", "%ppss" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", pps2, ov, new Vari("i32", "0"), new Vari("i32", "2")));
                                    local.llvm.func.comps.Add(new Store(pps2, go_v));
                                    hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                    var root = new Vari("%RootNodeType*", "%rn");
                                    var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                    local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                                    var wh = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                    txp = new Vari("ptr", "%txp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("ptr", "%txv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(tx, txp));
                                    var eq = new Vari("i1", "%eq" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(eq, tx, new Vari("ptr", "null")));
                                    var l1 = new Lab("eq" + LLVM.n++);
                                    var l2 = new Lab("th" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                    local.llvm.func.comps.Add(l1);
                                    var stateptr = new Vari("i32*", "%stp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                    local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                    local.llvm.func.comps.Add(new Br(null, l2));
                                    local.llvm.func.comps.Add(l2);
                                    local.llvm.func.comps.Add(new Load(wh, new Vari("%WaitHandleType*", "@WaitHandle")));
                                    var ret = new Vari("%RowSet*", "%ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(ret, wh, thgc, hv));

                                    var rowsp = new Vari("%HashType**", "%rowsp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RowSet", rowsp, ret, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var rowsv = new Vari("%HashType*", "%rowsv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(rowsv, rowsp));
                                    var kvsp = new Vari("%ListType**", "%kvs" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%HashType", kvsp, rowsv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var kvs = new Vari("%ListType*", "%kvs" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(kvs, kvsp));
                                    var sizep = new Vari("i32*", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ListType", sizep, kvs, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var size = new Vari("i32", "%s" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(size, sizep));

                                    var equal = new Vari("i1", "%equal" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(equal, size, new Vari("i32", "0")));
                                    var lab1 = new Lab("coro_end" + LLVM.n++);
                                    var lab2 = new Lab("coro_ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(equal, lab1, lab2));
                                    local.llvm.func.comps.Add(lab2);


                                    var glf = new Vari("%GetListType", "%glf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(glf, new Vari("%GetListType*", "@GetList")));
                                    var op2 = new Vari("ptr", "%op" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(op2, glf, kvs, new Vari("i32", "0")));
                                    var ov2 = new Vari("%KeyValueType*", "%ov" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ov2, op2));
                                    var op3 = new Vari(modelobj.model + "**", "%op3" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%KeyValueType", op3, ov2, new Vari("i32", "0"), new Vari("i32", "2")));
                                    var ov3 = new Vari(modelobj.model + "*", "%ov3" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ov3, op3));

                                    local.llvm.func.comps.Add(new Br(null, lab1));
                                    local.llvm.func.comps.Add(lab1);
                                    var vv = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var dict = new Dictionary<String, IfValue>();
                                    dict.Add(lab2.name, new IfValue(lab2, ov3));
                                    dict.Add(local.labs.Last().name, new IfValue(local.labs.Last(), new Vari(modelobj.model + "*", "null")));
                                    local.llvm.func.comps.Add(new Phi(vv, dict));

                                    return new Value(modelobj, vv);
                                    //make Coroutine Queue;
                                    //push handle;
                                    //loop
                                }
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "FirstAwaitの()が指定されていません");
                        }
                        else if (word2.name == "SelectAwait")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                val = where;
                                String sql = "";
                                var ps = new Vari("ptr", "null");
                                int pcount = 0;
                                Vari v = null;
                                var blk1 = modelobj.draw.children[0] as Block;
                                String vstr = "id";
                                foreach (var kv in blk1.vmapA)
                                {
                                    if (kv.Value.type == ObjType.Variable)
                                    {
                                        var variable = kv.Value as Variable;
                                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                        if (variable.cls.type == ObjType.ModelObj)
                                        {
                                            vstr += ", " + kv.Key;
                                        }
                                    }
                                }
                                var blk2 = modelobj.draw.children[1] as Block;
                                foreach (var kv in blk2.vmapA)
                                {
                                    if (kv.Value.type == ObjType.Variable)
                                    {
                                        if (kv.Key == "id") continue;
                                        var variable = kv.Value as Variable;
                                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                        if (variable.cls.type == ObjType.ModelObj)
                                        {
                                            vstr += ", " + kv.Key;
                                        }
                                    }
                                }
                                if (where == null)
                                {
                                    sql = "select " + vstr + " from " + modelobj.TableName() + ";";
                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                    local.llvm.strs.Add(sv);
                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                }
                                else if (val.type == ObjType.SqlString)
                                {
                                    var sqs = val as SqlString;
                                    sqs.Format();
                                    sql = "select " + vstr + " from " + modelobj.TableName() + " where " + sqs.value + ";";
                                    pcount = sqs.varis.Count;
                                    ps = new Vari("ptr", "%ps");
                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i32", (24 * pcount).ToString())));
                                    var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                    var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                    var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                    local.llvm.strs.Add(sv);
                                    local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    v = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                    local.llvm.func.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * sqs.varis.Count).ToString())));
                                    foreach (var kv in sqs.varis)
                                    {
                                        if (kv.Value.type == ObjType.Value)
                                        {
                                            var value = kv.Value as Value;
                                            if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                            if (value.cls.type == ObjType.ModelObj)
                                            {
                                                if (value.cls == local.Int)
                                                {
                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                                    var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    local.llvm.func.comps.Add(new Store(vptr, value.vari));
                                                }
                                                else if (value.cls == local.Str)
                                                {
                                                    var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("[" + sqs.varis.Count + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", kv.Key.ToString())));
                                                    var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                                    local.llvm.func.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                                    var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                                    //StringToUTF8
                                                    var utf8f = new Vari("%StringUTF8", "%utf8f");
                                                    local.llvm.func.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                                    var uc = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Alloca(uc));
                                                    var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Call(utf8v, utf8f, value.vari, uc));
                                                    local.llvm.func.comps.Add(new Store(vptr, utf8v));
                                                    var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                                    var uv = new Vari("i32", "%v" + LLVM.n++);
                                                    local.llvm.func.comps.Add(new Load(uv, uc));
                                                    local.llvm.func.comps.Add(new Store(lptr, uv));
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (val.type == ObjType.Value)
                                {
                                    var value = val as Value;
                                    ps = new Vari("ptr", "null");
                                    if (value.cls == local.Str)
                                    {
                                        sql = "select " + vstr + " from " + modelobj.TableName() + " where ";
                                        var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                                        var sv = new StrV("@s" + LLVM.n++, sql, sql.Length);
                                        local.llvm.strs.Add(sv);
                                        local.llvm.func.comps.Add(new Gete("[" + (sql.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                                        v = new Vari("%StringType*", "%v" + LLVM.n++);
                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        local.llvm.func.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", sql.Length.ToString()), new Vari("i32", 1.ToString())));
                                        var asv = new Vari("%AddStringType2", "%ad" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Load(asv, new Vari("%AddStringType2*", "@AddString2")));
                                        var strv = new Vari("%StringType*", "%str" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(strv, asv, thgc4, v, value.vari));
                                        //+ limit 1;
                                        v = strv;
                                    }
                                    else return Obj.Error(ObjType.Error, value.letter, "abc");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数はSqlString型かString型です");
                                if (local.llvm.func.async)
                                {
                                    var tx = new Vari("i8*", "%tx" + LLVM.n++);
                                    var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, new Vari("%RootNodeType*", "%rn"), new Vari("i32", "0"), new Vari("i32", "2")));
                                    local.llvm.func.comps.Add(new Load(tx, txp));
                                    for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                                    local.comps = new List<Component>();
                                    local.llvm.func.vari.type = "%FuncType**";
                                    var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var load = new Load(fv, local.llvm.func.vari);
                                    local.llvm.func.comps.Add(load);
                                    var go_v = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var rn4 = new Vari("%RootNodeType*", "%rn");
                                    var go_call = new Call(go_v, new Vari(modelobj.model + "*", modelobj.drawcall), rn4, fv);
                                    local.llvm.func.comps.Add(go_call);
                                    modelobj.Select(local);
                                    //Select * from users where (where) limit 1;
                                    var esql = new Vari("%ExecSqlType", "%esql");
                                    local.llvm.func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                    var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                    local.llvm.func.comps.Add(new Alloca(coroptr));
                                    local.llvm.func.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), go_v));
                                    var hv = new Vari("%CoroFrameType*", "%corov");
                                    local.llvm.func.comps.Add(new Load(hv, coroptr));
                                    var stateptr = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                    local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "0")));

                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var thgc = new Vari("%ThreadGCType*", "%thgc");
                                    var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(qv2, qv));
                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                    local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var nvari = new Vari("i32", (LLVM.n++).ToString());
                                    local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                    local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                    var lab6 = new Lab("aw" + LLVM.n++);
                                    local.llvm.func.comps.Add(lab6);
                                    var gev0 = new Vari(modelobj.model + "**", "%gv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", gev0, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                                    var gev02 = new Vari(modelobj.model + "*", "%gv2" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(gev02, gev0));
                                    var objptr0 = new Vari(modelobj.model + "**", "%objptr" + LLVM.n++);
                                    var alloca0 = new Alloca(objptr0);
                                    local.llvm.func.comps.Add(alloca0);
                                    var objstore0 = new Store(objptr0, gev02);
                                    local.llvm.func.comps.Add(objstore0);
                                    local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                    var retp = new Vari("i8**", "%ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", retp, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var ret = new Vari("i8*", "%retv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ret, retp));

                                    var rowsp = new Vari("HashType**", "%rowsp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RowSet", rowsp, ret, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var rowsv = new Vari("%HashType*", "%rowsv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(rowsv, rowsp));
                                    return new Value(modelobj, rowsv);
                                }
                                else
                                {
                                    var obj = new Vari("%CoroFrameType*", "%frame");
                                    var func = new Func(local.llvm, new Vari("void", "@First" + LLVM.n++), obj);
                                    local.llvm.comps.Add(func);
                                    func.async = true;

                                    var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                    var rngv = new Vari("%RootNodeType**", "%rngv");
                                    func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                    var rn4 = new Vari("%RootNodeType*", "%rn");
                                    func.comps.Add(new Load(rn4, rngv));
                                    func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                    var geterv = new Vari("i32*", "%state");
                                    func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var statev = new Vari("i32", "%statev" + LLVM.n++);
                                    func.comps.Add(new Load(statev, geterv));
                                    var list = new List<IfValue>();
                                    var lab10 = new Lab("coro_tx");
                                    func.sc = new SwitchComp(statev, lab10, list);
                                    func.comps.Add(func.sc);

                                    list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                    func.comps.Add(lab10);
                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var qvp = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv1 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv1, qvp));
                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv1));
                                    var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    var bt = new Vari("%BeginTransactionType", "%bt");
                                    var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                    func.comps.Add(new Alloca(alv));
                                    func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                    func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qv1));
                                    func.comps.Add(new Load(hvn, alv));
                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(qp, qv1));
                                    func.comps.Add(new Call(null, pushqueue1, qv1, hvn));
                                    lab10 = new Lab("coro_start" + LLVM.n++);
                                    list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                    var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                    var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                                    func.comps.Add(new Ret(new Vari("void", null)));
                                    func.comps.Add(lab10);

                                    var txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", txp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txps = new Vari("i8**", "%txps" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txps, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    func.comps.Add(new Store(txps, tx));
                                    var esql = new Vari("%ExecSqlType", "%esql");
                                    func.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                                    var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                                    func.comps.Add(new Alloca(coroptr));
                                    var op = new Vari("%Triptr**", "%op" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", op, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                    var obv = new Vari("%Triptr*", "%ov" + LLVM.n++);
                                    func.comps.Add(new Load(obv, op));
                                    var vp = new Vari("%StringType**", "%vp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", vp, obv, new Vari("i32", "0"), new Vari("i32", "0")));
                                    var v2 = new Vari("%StringType*", "%v" + LLVM.n++);
                                    func.comps.Add(new Load(v2, vp));
                                    var pps1 = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", pps1, obv, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var ps2 = new Vari("ptr", "%ps" + LLVM.n++);
                                    func.comps.Add(new Load(ps2, pps1));
                                    var sqpp = new Vari("ptr", "%sqpp" + LLVM.n++);
                                    func.comps.Add(new Gete("%Triptr", sqpp, obv, new Vari("i32", "0"), new Vari("i32", "2")));
                                    var sqp = new Vari("ptr", "%sqp" + LLVM.n++);
                                    func.comps.Add(new Load(sqp, sqpp));
                                    func.comps.Add(new Call(null, esql, coroptr, tx, v2, ps, new Vari("i32", pcount.ToString()), new Vari("ptr", "@DbSelect" + modelobj.identity), sqp));
                                    var hv = new Vari("%CoroFrameType*", "%corov");
                                    func.comps.Add(new Load(hv, coroptr));

                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    var thgc = new Vari("%ThreadGCType*", "%thgc");
                                    var qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    var sfinp2 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                    var sfinv2 = new Vari("i32", "%sfinv" + LLVM.n++);
                                    func.comps.Add(new Store(sfinv2, sfinp2));
                                    func.comps.Add(new Store(statevalptr2, sfinv2));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_txfin");

                                    list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                    func.comps.Add(lab10);

                                    var rp = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                                    var rv = new Vari("i8*", "%rv" + LLVM.n++);
                                    func.comps.Add(new Load(rv, rp));
                                    var rp2 = new Vari("i8**", "%rp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", rp2, obj, new Vari("i32", "0"), new Vari("i32", "9")));
                                    func.comps.Add(new Store(rp2, rv));
                                    txp = new Vari("i8**", "%tx" + LLVM.n++);
                                    func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("i8*", "%txv" + LLVM.n++);
                                    func.comps.Add(new Load(tx, txp));
                                    var txcf = new Vari("%TxFinishType", "%txcf" + LLVM.n++);
                                    func.comps.Add(new Load(txcf, new Vari("%TxFinishType*", "@TxCommit")));
                                    var hvp = new Vari("%CoroFrameType**", "%hv" + LLVM.n++);
                                    func.comps.Add(new Alloca(hvp));
                                    func.comps.Add(new Call(null, txcf, hvp, tx));
                                    hv = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                    func.comps.Add(new Load(hv, hvp));
                                    pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                    qv = new Vari("%CoroutineQueueType**", "%qvp" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                    qv2 = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                    func.comps.Add(new Load(qv2, qv));
                                    queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                    func.comps.Add(new Store(queueptr2, qv2));
                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                    statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));

                                    parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    lab10 = new Lab("coro_end");
                                    list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                    func.comps.Add(lab10);
                                    var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                    func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                    func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                    func.comps.Add(new Ret(new Vari("void", null)));

                                    var ov = new Vari("ptr", "%oj" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(ov, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                                    var sp = new Vari("%StringType**", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", sp, ov, new Vari("i32", "0"), new Vari("i32", "0")));
                                    local.llvm.func.comps.Add(new Store(sp, v));
                                    var pps = new Vari("%SqlParamType**", "%pps" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", pps, ov, new Vari("i32", "0"), new Vari("i32", "1")));
                                    local.llvm.func.comps.Add(new Store(pps, ps));
                                    for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
                                    local.comps = new List<Component>();
                                    local.llvm.func.vari.type = "%FuncType**";
                                    var fv = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var load = new Load(fv, local.llvm.func.vari);
                                    local.llvm.func.comps.Add(load);
                                    var go_v = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var go_call = new Call(go_v, new Vari(modelobj.model + "*", modelobj.drawcall), rn4, fv);
                                    local.llvm.func.comps.Add(go_call);
                                    modelobj.Select(local);
                                    var pps2 = new Vari("%SqlParamType**", "%ppss" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%Triptr", pps2, ov, new Vari("i32", "0"), new Vari("i32", "2")));
                                    local.llvm.func.comps.Add(new Store(pps2, go_v));
                                    hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                    var root = new Vari("%RootNodeType*", "%rn");
                                    var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                    local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                                    var wh = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                    txp = new Vari("ptr", "%txp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                    tx = new Vari("ptr", "%tx" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(tx, txp));
                                    var eq = new Vari("i1", "%eq" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(eq, tx, new Vari("ptr", "null")));
                                    var l1 = new Lab("eq" + LLVM.n++);
                                    var l2 = new Lab("th" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                    local.llvm.func.comps.Add(l1);
                                    var stateptr = new Vari("i32*", "%stp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                    local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                    local.llvm.func.comps.Add(new Br(null, l2));
                                    local.llvm.func.comps.Add(l2);
                                    local.llvm.func.comps.Add(new Load(wh, new Vari("%WaitHandleType*", "@WaitHandle")));
                                    var ret = new Vari("%RowSet*", "%ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(ret, wh, thgc, hv));

                                    var rowsp = new Vari("%ListType**", "%rowsp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%RowSet", rowsp, ret, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var rowsv = new Vari("%ListType*", "%rowsv" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(rowsv, rowsp));
                                    var sizep = new Vari("i32*", "%sp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ListType", sizep, rowsv, new Vari("i32", "0"), new Vari("i32", "1")));
                                    var size = new Vari("i32", "%s" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(size, sizep));

                                    var equal = new Vari("i1", "%equal" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Eq(equal, size, new Vari("i32", "0")));
                                    var lab1 = new Lab("coro_end" + LLVM.n++);
                                    var lab2 = new Lab("coro_ret" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Br(equal, lab1, lab2));
                                    local.llvm.func.comps.Add(lab2);


                                    var glf = new Vari("%GetListType", "%glf" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(glf, new Vari("%GetListType*", "@GetList")));
                                    var op2 = new Vari(modelobj.model + "**", "%op" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Call(op2, glf, rowsv, new Vari("i32", "0")));
                                    var ov2 = new Vari(modelobj.model + "*", "%ov" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Load(ov2, op2));

                                    local.llvm.func.comps.Add(new Br(null, lab1));
                                    local.llvm.func.comps.Add(lab1);
                                    var vv = new Vari(modelobj.model + "*", "%v" + LLVM.n++);
                                    var dict = new Dictionary<String, IfValue>();
                                    dict.Add(lab2.name, new IfValue(lab2, ov2));
                                    dict.Add(local.labs.Last().name, new IfValue(local.labs.Last(), new Vari(modelobj.model + "*", "null")));
                                    local.llvm.func.comps.Add(new Phi(vv, dict));

                                    return new Value(modelobj, vv);
                                    //make Coroutine Queue;
                                    //push handle;
                                    //loop
                                }
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "FirstAwaitの()が指定されていません");
                        }
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前が来ます");
                }
                else return new VoiVal();
            }
            else if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    val2 = primary.children[n];
                    if (word.name == "Store")
                    {
                        if (val2.type == ObjType.Dot)
                        {
                            n++;
                            val2 = primary.children[n];
                            if (val2.type == ObjType.Word)
                            {
                                var word2 = val2 as Word;
                                n++;
                                val2 = primary.children[n];
                                if (word2.name == "await")
                                {
                                    if (val2.type == ObjType.Bracket)
                                    {
                                        n++;
                                        var callname = "@TxStore" + LLVM.n++;
                                        this.vari = new Vari("void", callname);
                                        var obj = new Vari("%CoroFrameType*", "%frame");
                                        var func = new Func(local.llvm, this.vari, obj);
                                        local.llvm.comps.Add(func);
                                        func.async = true;

                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        var rngv = new Vari("%RootNodeType**", "%rngv");
                                        func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                        var rn4 = new Vari("%RootNodeType*", "%rn");
                                        func.comps.Add(new Load(rn4, rngv));
                                        func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                        var geterv = new Vari("i32*", "%state");
                                        func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                        var statev = new Vari("i32", "%statev" + LLVM.n++);
                                        func.comps.Add(new Load(statev, geterv));
                                        var list = new List<IfValue>();
                                        var lab10 = new Lab("coro_tx");
                                        func.sc = new SwitchComp(statev, lab10, list);
                                        func.comps.Add(func.sc);

                                        if (local.llvm.func.async) { }
                                        else
                                        {
                                            list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                            func.comps.Add(lab10);
                                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qvn, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                            func.comps.Add(new Load(qvn2, qvn));
                                            /*var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(queueptr2, qvn2));*/
                                            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                            var bt = new Vari("%BeginTransactionType", "%bt");
                                            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                            func.comps.Add(new Alloca(alv));
                                            func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                            func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qvn2));
                                            func.comps.Add(new Load(hvn, alv));
                                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(qp, qvn2));
                                            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                            var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                            func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));

                                        }
                                        var val = val2.exeC(local).GetterC(local);
                                        var blk = val as Block;
                                        bool first = true;
                                        var typename = "%store" + LLVM.n++;
                                        var typedec = new TypeDec(typename);
                                        local.llvm.types.Add(typedec);
                                        var countv = new Vari("i64", "");
                                        var objp = new Vari("ptr", "%objp" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(objp, new Vari("ptr", "@malloc"), countv));
                                        int count = 0;
                                        var n3 = 0;
                                        for (var i = 0; i < blk.rets.Count; i++)
                                        {
                                            if (blk.rets[i] is Value)
                                            {
                                                var value = blk.rets[i] as Value;
                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                if (value.cls.type == ObjType.ModelObj || value.cls.type == ObjType.GeneObj)
                                                {
                                                    if (first)
                                                    {
                                                        first = false;
                                                        lab10 = new Lab("coro_start" + LLVM.n++);
                                                        list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                                        if (local.llvm.func.async)
                                                        {
                                                            func.sc.lab = lab10;
                                                            func.comps.Add(new Ret(new Vari("void", null)));
                                                            func.comps.Add(lab10);
                                                        }
                                                        else
                                                        {
                                                            var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                            func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                                            func.comps.Add(new Ret(new Vari("void", null)));
                                                            func.comps.Add(lab10);
                                                            var cvp = new Vari("i8**", "%cvp");
                                                            func.comps.Add(new Gete("%CoroFrameType", cvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                                            var cv = new Vari("i8*", "%cv");
                                                            func.comps.Add(new Load(cv, cvp));
                                                            var txp = new Vari("i8**", "%txp");
                                                            func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                            func.comps.Add(new Store(txp, cv));
                                                        }
                                                    }
                                                    else
                                                    {
                                                        lab10 = new Lab("coro_start" + LLVM.n++);
                                                        int n2 = LLVM.n++;
                                                        list.Add(new IfValue(lab10, new Vari("i32", n2.ToString())));
                                                        var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                        func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                        func.comps.Add(new Store(statevalptr3, new Vari("i32", n2.ToString())));
                                                        func.comps.Add(new Ret(new Vari("void", null)));
                                                        func.comps.Add(new Ret(new Vari("void", null)));
                                                        func.comps.Add(lab10);
                                                    }
                                                    var model = value.cls as ModelObj;
                                                    model.Store(local);
                                                    var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                                                    func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    var tx = new Vari("i8*", "%tx" + LLVM.n++);
                                                    func.comps.Add(new Load(tx, txptr));

                                                    var objvp = new Vari(typename + "**", "%gv" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", objvp, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                                    var objv = new Vari(typename + "*", "%objv" + LLVM.n++);
                                                    func.comps.Add(new Load(objv, objvp));
                                                    Vari vx;
                                                    if (value.cls == local.Int)
                                                    {
                                                        typedec.comps.Add(new TypeVal("i32", i.ToString()));
                                                        var gv = new Vari("i32*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("i32*", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("i32", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;

                                                    }
                                                    else if (value.cls == local.Short)
                                                    {
                                                        typedec.comps.Add(new TypeVal("i16", i.ToString()));
                                                        var gv = new Vari("i16*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("i16*", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("i16", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;
                                                    }
                                                    else if (value.cls == local.Bool)
                                                    {
                                                        typedec.comps.Add(new TypeVal("i1", i.ToString()));
                                                        var gv = new Vari("i1*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("i1*", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("i1", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;
                                                    }
                                                    else if (value.cls == local.Str)
                                                    {
                                                        typedec.comps.Add(new TypeVal("%StringType*", i.ToString()));
                                                        var gv = new Vari("%StringType*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("%StringType**", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        var vx0 = new Vari("%StringType*", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx0, vxp));
                                                        var utf8 = new Vari("%StringUTF8Type*", "%utf8" + LLVM.n++);
                                                        func.comps.Add(new Load(utf8, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                                        var len = new Vari("i32*", "%len" + LLVM.n++);
                                                        func.comps.Add(new Alloca(len));
                                                        vx = new Vari("i8*", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Call(vx, utf8, vx0, len));
                                                        count += 8;
                                                    }
                                                    else
                                                    {
                                                        typedec.comps.Add(new TypeVal("%GCModel*", i.ToString()));
                                                        var gv = new Vari("%GCModel**", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("%GCModel**", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("%GCModel*", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;
                                                    }
                                                    var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                                    func.comps.Add(new Call(new Vari("ptr", hv.name), new Vari("ptr", "@DbStore" + model.identity), tx, vx));
                                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                    var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                                    func.comps.Add(new Load(qv2, qv));
                                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                    func.comps.Add(new Store(queueptr2, qv2));
                                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));

                                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                    n3++;
                                                }
                                                else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数にはオブジェクトクラスかGeneオブジェクトを指定してください");
                                            }
                                            else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数には値を指定してください");
                                        }
                                        countv.name = count.ToString();
                                        if (local.llvm.func.async)
                                        {
                                            var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            lab10 = new Lab("coro_fin");
                                            list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                            func.comps.Add(lab10);
                                            var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                            var parent = new Vari("%CoroFrameType**", "%parent");
                                            func.comps.Add(new Gete("%CoroFrameType", parent, obj, new Vari("i32", "0"), new Vari("i32", "6")));
                                            var handle = new Vari("%CoroFrameType*", "%frame" + LLVM.n++);
                                            func.comps.Add(new Load(handle, parent));
                                            var comp = new Vari("i1", "%comp");
                                            func.comps.Add(new Ne(comp, handle, new Vari("ptr", "null")));
                                            var lab = new Lab("coro_end" + identity);
                                            var lab2 = new Lab("coro_ret");
                                            func.comps.Add(new Br(comp, lab, lab2));
                                            func.comps.Add(lab);
                                            var queueptr = new Vari("%CoroutineQueueType**", "%queueptr");
                                            func.comps.Add(new Gete("%CoroFrameType", queueptr, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var queue = new Vari("%PushQueueType*", "%queue");
                                            func.comps.Add(new Load(queue, queueptr));
                                            var pushqueue = new Vari("%PushQueueType*", "%pushqueue");
                                            func.comps.Add(new Load(pushqueue, new Vari("%PushQueueType*", "@PushQueue")));
                                            func.comps.Add(new Call(null, pushqueue, queue, handle));
                                            func.comps.Add(new Br(null, lab2));
                                            func.comps.Add(lab2);
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            var hv = new Vari("%CoroFrameType*", "%corov" + LLVM.n++);
                                            var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                            local.llvm.func.comps.Add(new Call(hv, mkf, new Vari("%RootNodeType*", "%rn"), new Vari("ptr", func.y.name), objp));
                                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                            var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", qv, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(qv2, qv));
                                            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                            local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                            var stateptr = new Vari("i32*", "%sp" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                            local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "0")));
                                            local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                            statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            var nvari = new Vari("i32", LLVM.n++.ToString());
                                            local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                            local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                            var lab6 = new Lab("aw" + LLVM.n++);
                                            local.llvm.func.comps.Add(lab6);
                                            var gev0 = new Vari("ptr", "%gv" + LLVM.n++);
                                            local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                        }
                                        else
                                        {
                                            var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                            var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                                            func.comps.Add(new Load(sfinv, sfinp));
                                            func.comps.Add(new Store(statevalptr2, sfinv));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            lab10 = new Lab("coro_end");
                                            list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                            func.comps.Add(lab10);
                                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qvn, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                            func.comps.Add(new Load(qvn2, qvn));
                                            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(queueptr2, qvn2));
                                            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                            var bt = new Vari("%TxFinishType", "%tf");
                                            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                            func.comps.Add(new Alloca(alv));
                                            func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
                                            var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                            func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            var txv = new Vari("i8*", "%txv" + LLVM.n++);
                                            func.comps.Add(new Load(txv, txp));
                                            func.comps.Add(new Call(null, bt, alv, txv));
                                            func.comps.Add(new Load(hvn, alv));
                                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(qp, qvn2));
                                            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                            statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            lab10 = new Lab("coro_fin");
                                            list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                            func.comps.Add(lab10);
                                            var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            var hv = new Vari("%CoroFrameType*", "%corov" + LLVM.n++);
                                            var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                            local.llvm.func.comps.Add(new Call(hv, mkf, new Vari("%RootNodeType*", "%rn"), new Vari("ptr", func.y.name), objp));
                                            txp = new Vari("ptr", "%txp" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            txv = new Vari("ptr", "%tx" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(txv, txp));
                                            var eq = new Vari("i1", "%eq" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Eq(eq, txv, new Vari("ptr", "null")));
                                            var l1 = new Lab("eq" + LLVM.n++);
                                            var l2 = new Lab("th" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                            local.llvm.func.comps.Add(l1);
                                            var stateptr = new Vari("i32*", "%sp" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                            local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                            local.llvm.func.comps.Add(new Br(null, l2));
                                            local.llvm.func.comps.Add(l2);
                                            var waithandle = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(waithandle, new Vari("%WaitHandleType*", "@WaitHandle")));
                                            var ret = new Vari("i8*", "%ret" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(ret, waithandle, thgc4, hv));
                                            this.vari = ret;
                                        }
                                        return new VoiVal();
                                    }
                                }
                                else Obj.Error(ObjType.Error, val2.letter, "Storeの.の後にawaitがありません");
                            }
                            else return new VoiVal();
                        }
                        else if (val2.type == ObjType.Bracket)
                        {
                            n++;
                            var callname = "@TxStore" + LLVM.n++;
                            this.vari = new Vari("void", callname);
                            var obj = new Vari("%CoroFrameType*", "%frame");
                            var func = new Func(local.llvm, this.vari, obj);
                            local.llvm.comps.Add(func);
                            func.async = true;

                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                            var rngv = new Vari("%RootNodeType**", "%rngv");
                            func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                            var rn4 = new Vari("%RootNodeType*", "%rn");
                            func.comps.Add(new Load(rn4, rngv));
                            func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                            var geterv = new Vari("i32*", "%state");
                            func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                            var statev = new Vari("i32", "%statev" + LLVM.n++);
                            func.comps.Add(new Load(statev, geterv));
                            var list = new List<IfValue>();
                            var lab10 = new Lab("coro_tx");
                            func.sc = new SwitchComp(statev, lab10, list);
                            func.comps.Add(func.sc);

                            list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                            func.comps.Add(lab10);
                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                            func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                            func.comps.Add(new Load(qvn2, qvn));
                            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                            func.comps.Add(new Store(queueptr2, qvn2));
                            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                            var bt = new Vari("%BeginTransactionType", "%bt");
                            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                            func.comps.Add(new Alloca(alv));
                            func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                            func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qvn2));
                            func.comps.Add(new Load(hvn, alv));
                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                            func.comps.Add(new Store(qp, qvn2));
                            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                            var sfinp = new Vari("i32*", "%sfin" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                            func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                            var val = val2.exeC(local).GetterC(local);
                            var blk = val as Block;
                            bool first = true;
                            var typename = "%store" + LLVM.n++;
                            var typedec = new TypeDec(typename);
                            local.llvm.types.Add(typedec);
                            var countv = new Vari("i64", "");
                            var objp = new Vari("ptr", "%objp" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(objp, new Vari("ptr", "@malloc"), countv));
                            int count = 0;
                            var n3 = 0;
                            for (var i = 0; i < blk.rets.Count; i++)
                            {
                                if (blk.rets[i] is Value)
                                {
                                    var value = blk.rets[i] as Value;
                                    if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                    if (value.cls.type == ObjType.ModelObj || value.cls.type == ObjType.GeneObj)
                                    {
                                        lab10 = new Lab("coro_start" + LLVM.n++);
                                        int n2 = LLVM.n++;
                                        list.Add(new IfValue(lab10, new Vari("i32", n2.ToString())));
                                        var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                        func.comps.Add(new Store(statevalptr3, new Vari("i32", n2.ToString())));
                                        func.comps.Add(new Ret(new Vari("void", null)));
                                        func.comps.Add(lab10);
                                        var model = value.cls as ModelObj;
                                        model.Store(local);
                                        Vari tx;
                                        if (n3 == 0)
                                        {
                                            var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", txp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                            tx = new Vari("i8*", "%txv" + LLVM.n++);
                                            func.comps.Add(new Load(tx, txp));
                                            var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                                            func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            func.comps.Add(new Store(txptr, tx));

                                        }
                                        else
                                        {
                                            var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                                            func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            tx = new Vari("i8*", "%tx" + LLVM.n++);
                                            func.comps.Add(new Load(tx, txptr));
                                        }

                                        var objvp = new Vari(typename + "**", "%gv" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", objvp, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                        var objv = new Vari(typename + "*", "%objv" + LLVM.n++);
                                        func.comps.Add(new Load(objv, objvp));
                                        Vari vx;
                                        if (value.cls == local.Int)
                                        {
                                            typedec.comps.Add(new TypeVal("i32", i.ToString()));
                                            var gv = new Vari("i32*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("i32*", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("i32", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;

                                        }
                                        else if (value.cls == local.Short)
                                        {
                                            typedec.comps.Add(new TypeVal("i16", i.ToString()));
                                            var gv = new Vari("i16*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("i16*", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("i16", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;
                                        }
                                        else if (value.cls == local.Bool)
                                        {
                                            typedec.comps.Add(new TypeVal("i1", i.ToString()));
                                            var gv = new Vari("i1*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("i1*", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("i1", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;
                                        }
                                        else if (value.cls == local.Str)
                                        {
                                            typedec.comps.Add(new TypeVal("%StringType*", i.ToString()));
                                            var gv = new Vari("%StringType*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("%StringType**", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            var vx0 = new Vari("%StringType*", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx0, vxp));
                                            var utf8 = new Vari("%StringUTF8Type*", "%utf8" + LLVM.n++);
                                            func.comps.Add(new Load(utf8, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                            var len = new Vari("i32*", "%len" + LLVM.n++);
                                            func.comps.Add(new Alloca(len));
                                            vx = new Vari("i8*", "%vx" + LLVM.n++);
                                            func.comps.Add(new Call(vx, utf8, vx0, len));
                                            count += 8;
                                        }
                                        else
                                        {
                                            typedec.comps.Add(new TypeVal("%GCModel*", i.ToString()));
                                            var gv = new Vari("%GCModel**", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("%GCModel**", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("%GCModel*", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;
                                        }
                                        var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                        func.comps.Add(new Call(new Vari("ptr", hv.name), new Vari("ptr", "@DbStore" + model.identity), tx, vx));
                                        pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                        func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                        var thgc = new Vari("%ThreadGCType*", "%thgc");
                                        var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                        func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                        var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                        func.comps.Add(new Load(qv2, qv));
                                        queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                        func.comps.Add(new Store(queueptr2, qv2));
                                        func.comps.Add(new Call(null, pushqueue1, qv2, hv));

                                        parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                        func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                        n3++;
                                    }
                                    else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数にはオブジェクトクラスかGeneオブジェクトを指定してください");
                                }
                                else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数には値を指定してください");
                            }
                            countv.name = count.ToString();
                            {
                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                var sfinp2 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                var sfinv2 = new Vari("i32", "%sfinv" + LLVM.n++);
                                func.comps.Add(new Load(sfinv2, sfinp2));
                                func.comps.Add(new Store(statevalptr2, sfinv2));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_end");
                                list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                func.comps.Add(lab10);
                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                                qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                func.comps.Add(new Load(qvn2, qvn));
                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(queueptr2, qvn2));
                                hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                bt = new Vari("%TxFinishType", "%tf");
                                alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                func.comps.Add(new Alloca(alv));
                                func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
                                var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                var txv = new Vari("i8*", "%txv" + LLVM.n++);
                                func.comps.Add(new Load(txv, txp));
                                func.comps.Add(new Call(null, bt, alv, txv));
                                func.comps.Add(new Load(hvn, alv));
                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(qp, qvn2));
                                func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_fin");
                                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                func.comps.Add(lab10);
                                var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                var gcr = new Vari("%GC_AddRootType", "%gcr" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(gcr, new Vari("%GC_AddRootType*", "@GC_AddRoot")));
                                var rv = new Vari("%RootNodeType*", "%rv" + LLVM.n++);
                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                local.llvm.func.comps.Add(new Call(rv, gcr, thgc));
                                var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, rv, new Vari("ptr", func.y.name), objp));
                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(qv2, qv));
                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                txp = new Vari("ptr", "%txp" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                txv = new Vari("ptr", "%tx" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(txv, txp));
                                var eq = new Vari("i1", "%eq" + LLVM.n++);
                                local.llvm.func.comps.Add(new Eq(eq, txv, new Vari("ptr", "null")));
                                var l1 = new Lab("eq" + LLVM.n++);
                                var l2 = new Lab("th" + LLVM.n++);
                                local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                local.llvm.func.comps.Add(l1);
                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                local.llvm.func.comps.Add(new Store(statevalptr2, new Vari("i32", "-2")));
                                local.llvm.func.comps.Add(new Br(null, l2));
                                local.llvm.func.comps.Add(l2);

                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                local.llvm.func.comps.Add(new Store(parent2, hv));
                                //local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                            }
                            return new VoiVal();
                        }
                    }
                    else if (word.name == "Delete")
                        {
                        if (val2.type == ObjType.Dot)
                        {
                            n++;
                            val2 = primary.children[n];
                            if (val2.type == ObjType.Word)
                            {
                                var word2 = val2 as Word;
                                n++;
                                val2 = primary.children[n];
                                if (word2.name == "await")
                                {
                                    if (val2.type == ObjType.Bracket)
                                    {
                                        n++;
                                        var callname = "@TxStore" + LLVM.n++;
                                        this.vari = new Vari("void", callname);
                                        var obj = new Vari("%CoroFrameType*", "%frame");
                                        var func = new Func(local.llvm, this.vari, obj);
                                        local.llvm.comps.Add(func);
                                        func.async = true;

                                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                                        var rngv = new Vari("%RootNodeType**", "%rngv");
                                        func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                                        var rn4 = new Vari("%RootNodeType*", "%rn");
                                        func.comps.Add(new Load(rn4, rngv));
                                        func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                                        var geterv = new Vari("i32*", "%state");
                                        func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                        var statev = new Vari("i32", "%statev" + LLVM.n++);
                                        func.comps.Add(new Load(statev, geterv));
                                        var list = new List<IfValue>();
                                        var lab10 = new Lab("coro_tx");
                                        func.sc = new SwitchComp(statev, lab10, list);
                                        func.comps.Add(func.sc);

                                        if (local.llvm.func.async) { }
                                        else
                                        {
                                            list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                                            func.comps.Add(lab10);
                                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qvn, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                            func.comps.Add(new Load(qvn2, qvn));
                                            /*var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(queueptr2, qvn2));*/
                                            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                            var bt = new Vari("%BeginTransactionType", "%bt");
                                            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                            func.comps.Add(new Alloca(alv));
                                            func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                            func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qvn2));
                                            func.comps.Add(new Load(hvn, alv));
                                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(qp, qvn2));
                                            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                            var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                            func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                                        }
                                        var val = val2.exeC(local).GetterC(local);
                                        var blk = val as Block;
                                        bool first = true;
                                        var typename = "%store" + LLVM.n++;
                                        var typedec = new TypeDec(typename);
                                        local.llvm.types.Add(typedec);
                                        var countv = new Vari("i64", "");
                                        var objp = new Vari("ptr", "%objp" + LLVM.n++);
                                        local.llvm.func.comps.Add(new Call(objp, new Vari("ptr", "@malloc"), countv));
                                        int count = 0;
                                        var n3 = 0;
                                        for (var i = 0; i < blk.rets.Count; i++)
                                        {
                                            if (blk.rets[i] is Value)
                                            {
                                                var value = blk.rets[i] as Value;
                                                if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                                if (value.cls.type == ObjType.ModelObj || value.cls.type == ObjType.GeneObj)
                                                {
                                                    if (first)
                                                    {
                                                        first = false;
                                                        lab10 = new Lab("coro_start" + LLVM.n++);
                                                        list.Add(new IfValue(lab10, new Vari("i32", "0")));
                                                        if (local.llvm.func.async)
                                                        {
                                                            func.sc.lab = lab10;
                                                            func.comps.Add(new Ret(new Vari("void", null)));
                                                            func.comps.Add(lab10);
                                                        }
                                                        else
                                                        {
                                                            var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                            func.comps.Add(new Store(statevalptr3, new Vari("i32", "0")));
                                                            func.comps.Add(new Ret(new Vari("void", null)));
                                                            func.comps.Add(lab10);
                                                            var cvp = new Vari("i8**", "%cvp");
                                                            func.comps.Add(new Gete("%CoroFrameType", cvp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                                            var cv = new Vari("i8*", "%cv");
                                                            func.comps.Add(new Load(cv, cvp));
                                                            var txp = new Vari("i8**", "%txp");
                                                            func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                            func.comps.Add(new Store(txp, cv));
                                                        }
                                                    }
                                                    else
                                                    {
                                                        lab10 = new Lab("coro_start" + LLVM.n++);
                                                        int n2 = LLVM.n++;
                                                        list.Add(new IfValue(lab10, new Vari("i32", n2.ToString())));
                                                        var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                                        func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                                        func.comps.Add(new Store(statevalptr3, new Vari("i32", n2.ToString())));
                                                        func.comps.Add(new Ret(new Vari("void", null)));
                                                        func.comps.Add(new Ret(new Vari("void", null)));
                                                        func.comps.Add(lab10);
                                                    }
                                                    var model = value.cls as ModelObj;
                                                    model.Delete(local);
                                                    var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                                                    func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                                    var tx = new Vari("i8*", "%tx" + LLVM.n++);
                                                    func.comps.Add(new Load(tx, txptr));

                                                    var objvp = new Vari(typename + "**", "%gv" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", objvp, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                                    var objv = new Vari(typename + "*", "%objv" + LLVM.n++);
                                                    func.comps.Add(new Load(objv, objvp));
                                                    Vari vx;
                                                    if (value.cls == local.Int)
                                                    {
                                                        typedec.comps.Add(new TypeVal("i32", i.ToString()));
                                                        var gv = new Vari("i32*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("i32*", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("i32", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;

                                                    }
                                                    else if (value.cls == local.Short)
                                                    {
                                                        typedec.comps.Add(new TypeVal("i16", i.ToString()));
                                                        var gv = new Vari("i16*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("i16*", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("i16", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;
                                                    }
                                                    else if (value.cls == local.Bool)
                                                    {
                                                        typedec.comps.Add(new TypeVal("i1", i.ToString()));
                                                        var gv = new Vari("i1*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("i1*", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("i1", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;
                                                    }
                                                    else if (value.cls == local.Str)
                                                    {
                                                        typedec.comps.Add(new TypeVal("%StringType*", i.ToString()));
                                                        var gv = new Vari("%StringType*", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("%StringType**", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        var vx0 = new Vari("%StringType*", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx0, vxp));
                                                        var utf8 = new Vari("%StringUTF8Type*", "%utf8" + LLVM.n++);
                                                        func.comps.Add(new Load(utf8, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                                        var len = new Vari("i32*", "%len" + LLVM.n++);
                                                        func.comps.Add(new Alloca(len));
                                                        vx = new Vari("i8*", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Call(vx, utf8, vx0, len));
                                                        count += 8;
                                                    }
                                                    else
                                                    {
                                                        typedec.comps.Add(new TypeVal("%GCModel*", i.ToString()));
                                                        var gv = new Vari("%GCModel**", "%gv" + LLVM.n++);
                                                        local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        local.llvm.func.comps.Add(new Store(gv, value.vari));
                                                        var vxp = new Vari("%GCModel**", "%vxp" + LLVM.n++);
                                                        func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                                        vx = new Vari("%GCModel*", "%vx" + LLVM.n++);
                                                        func.comps.Add(new Load(vx, vxp));
                                                        count += 8;
                                                    }
                                                    var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                                    func.comps.Add(new Call(new Vari("ptr", hv.name), new Vari("ptr", "@DbDelete" + model.identity), tx, vx));
                                                    var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                                    func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                                    var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", qv, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                                    var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                                    func.comps.Add(new Load(qv2, qv));
                                                    var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                                    func.comps.Add(new Store(queueptr2, qv2));
                                                    func.comps.Add(new Call(null, pushqueue1, qv2, hv));

                                                    var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                                    func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                                    func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                                    n3++;
                                                }
                                                else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数にはオブジェクトクラスかGeneオブジェクトを指定してください");
                                            }
                                            else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数には値を指定してください");
                                        }
                                        countv.name = count.ToString();
                                        if (local.llvm.func.async)
                                        {
                                            var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            lab10 = new Lab("coro_fin");
                                            list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                            func.comps.Add(lab10);
                                            var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                            var parent = new Vari("%CoroFrameType**", "%parent");
                                            func.comps.Add(new Gete("%CoroFrameType", parent, obj, new Vari("i32", "0"), new Vari("i32", "6")));
                                            var handle = new Vari("%CoroFrameType*", "%frame" + LLVM.n++);
                                            func.comps.Add(new Load(handle, parent));
                                            var comp = new Vari("i1", "%comp");
                                            func.comps.Add(new Ne(comp, handle, new Vari("ptr", "null")));
                                            var lab = new Lab("coro_end" + identity);
                                            var lab2 = new Lab("coro_ret");
                                            func.comps.Add(new Br(comp, lab, lab2));
                                            func.comps.Add(lab);
                                            var queueptr = new Vari("%CoroutineQueueType**", "%queueptr");
                                            func.comps.Add(new Gete("%CoroFrameType", queueptr, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var queue = new Vari("%PushQueueType*", "%queue");
                                            func.comps.Add(new Load(queue, queueptr));
                                            var pushqueue = new Vari("%PushQueueType*", "%pushqueue");
                                            func.comps.Add(new Load(pushqueue, new Vari("%PushQueueType*", "@PushQueue")));
                                            func.comps.Add(new Call(null, pushqueue, queue, handle));
                                            func.comps.Add(new Br(null, lab2));
                                            func.comps.Add(lab2);
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            var hv = new Vari("%CoroFrameType*", "%corov" + LLVM.n++);
                                            var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                            local.llvm.func.comps.Add(new Call(hv, mkf, new Vari("%RootNodeType*", "%rn"), new Vari("ptr", func.y.name), objp));
                                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                            var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", qv, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(qv2, qv));
                                            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                            local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                            var stateptr = new Vari("i32*", "%sp" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                            local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "0")));
                                            local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                            statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            var nvari = new Vari("i32", (LLVM.n++).ToString());
                                            local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                            local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                            var lab6 = new Lab("aw" + LLVM.n++);
                                            local.llvm.func.comps.Add(lab6);
                                            var gev0 = new Vari("ptr", "%gv" + LLVM.n++);
                                            local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                        }
                                        else
                                        {
                                            var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                            var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                                            func.comps.Add(new Load(sfinv, sfinp));
                                            func.comps.Add(new Store(statevalptr2, sfinv));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            lab10 = new Lab("coro_end");
                                            list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                            func.comps.Add(lab10);
                                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qvn, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "4")));
                                            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                            func.comps.Add(new Load(qvn2, qvn));
                                            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(queueptr2, qvn2));
                                            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                            var bt = new Vari("%TxFinishType", "%tf");
                                            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                            func.comps.Add(new Alloca(alv));
                                            func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
                                            var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                            func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            var txv = new Vari("i8*", "%txv" + LLVM.n++);
                                            func.comps.Add(new Load(txv, txp));
                                            func.comps.Add(new Call(null, bt, alv, txv));
                                            func.comps.Add(new Load(hvn, alv));
                                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                            func.comps.Add(new Store(qp, qvn2));
                                            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                            statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            lab10 = new Lab("coro_fin");
                                            list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                            func.comps.Add(lab10);
                                            var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                            func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                            func.comps.Add(new Ret(new Vari("void", null)));
                                            var hv = new Vari("%CoroFrameType*", "%corov" + LLVM.n++);
                                            var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                            local.llvm.func.comps.Add(new Call(hv, mkf, new Vari("%RootNodeType*", "%rn"), new Vari("ptr", func.y.name), objp));
                                            txp = new Vari("ptr", "%txp" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            txv = new Vari("ptr", "%tx" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(txv, txp));
                                            var eq = new Vari("i1", "%eq" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Eq(eq, txv, new Vari("ptr", "null")));
                                            var l1 = new Lab("eq" + LLVM.n++);
                                            var l2 = new Lab("th" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                            local.llvm.func.comps.Add(l1);
                                            var stateptr = new Vari("i32*", "%sp" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateptr, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                            local.llvm.func.comps.Add(new Store(stateptr, new Vari("i32", "-2")));
                                            local.llvm.func.comps.Add(new Br(null, l2));
                                            local.llvm.func.comps.Add(l2);
                                            var waithandle = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Load(waithandle, new Vari("%WaitHandleType*", "@WaitHandle")));
                                            var ret = new Vari("i8*", "%ret" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Call(ret, waithandle, thgc4, hv));
                                            this.vari = ret;
                                        }
                                        return new VoiVal();
                                    }
                                }
                                else Obj.Error(ObjType.Error, val2.letter, "Storeの.の後にawaitがありません");
                            }
                            else return new VoiVal();
                        }
                        else if (val2.type == ObjType.Bracket)
                        {
                            n++;
                            var callname = "@TxStore" + LLVM.n++;
                            this.vari = new Vari("void", callname);
                            var obj = new Vari("%CoroFrameType*", "%frame");
                            var func = new Func(local.llvm, this.vari, obj);
                            local.llvm.comps.Add(func);
                            func.async = true;

                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                            var rngv = new Vari("%RootNodeType**", "%rngv");
                            func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                            var rn4 = new Vari("%RootNodeType*", "%rn");
                            func.comps.Add(new Load(rn4, rngv));
                            func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                            var geterv = new Vari("i32*", "%state");
                            func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                            var statev = new Vari("i32", "%statev" + LLVM.n++);
                            func.comps.Add(new Load(statev, geterv));
                            var list = new List<IfValue>();
                            var lab10 = new Lab("coro_tx");
                            func.sc = new SwitchComp(statev, lab10, list);
                            func.comps.Add(func.sc);

                            list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                            func.comps.Add(lab10);
                            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                            func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                            func.comps.Add(new Load(qvn2, qvn));
                            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                            func.comps.Add(new Store(queueptr2, qvn2));
                            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                            var bt = new Vari("%BeginTransactionType", "%bt");
                            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                            func.comps.Add(new Alloca(alv));
                            func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                            func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qvn2));
                            func.comps.Add(new Load(hvn, alv));
                            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                            func.comps.Add(new Store(qp, qvn2));
                            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                            var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                            func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                            func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                            var val = val2.exeC(local).GetterC(local);
                            var blk = val as Block;
                            bool first = true;
                            var typename = "%store" + LLVM.n++;
                            var typedec = new TypeDec(typename);
                            local.llvm.types.Add(typedec);
                            var countv = new Vari("i64", "");
                            var objp = new Vari("ptr", "%objp" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(objp, new Vari("ptr", "@malloc"), countv));
                            int count = 0;
                            var n3 = 0;
                            for (var i = 0; i < blk.rets.Count; i++)
                            {
                                if (blk.rets[i] is Value)
                                {
                                    var value = blk.rets[i] as Value;
                                    if (value.cls.type == ObjType.Var) value.cls = (value.cls as Var).cls;
                                    if (value.cls.type == ObjType.ModelObj || value.cls.type == ObjType.GeneObj)
                                    {
                                        lab10 = new Lab("coro_start" + LLVM.n++);
                                        int n2 = LLVM.n++;
                                        list.Add(new IfValue(lab10, new Vari("i32", n2.ToString())));
                                        var statevalptr3 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", statevalptr3, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                        func.comps.Add(new Store(statevalptr3, new Vari("i32", n2.ToString())));
                                        func.comps.Add(new Ret(new Vari("void", null)));
                                        func.comps.Add(lab10);
                                        var model = value.cls as ModelObj;
                                        model.Store(local);
                                        Vari tx;
                                        if (n3 == 0)
                                        {
                                            var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                            func.comps.Add(new Gete("%CoroFrameType", txp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                                            tx = new Vari("i8*", "%txv" + LLVM.n++);
                                            func.comps.Add(new Load(tx, txp));
                                            var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                                            func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            func.comps.Add(new Store(txptr, tx));

                                        }
                                        else
                                        {
                                            var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                                            func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                            tx = new Vari("i8*", "%tx" + LLVM.n++);
                                            func.comps.Add(new Load(tx, txptr));
                                        }

                                        var objvp = new Vari(typename + "**", "%gv" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", objvp, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                        var objv = new Vari(typename + "*", "%objv" + LLVM.n++);
                                        func.comps.Add(new Load(objv, objvp));
                                        Vari vx;
                                        if (value.cls == local.Int)
                                        {
                                            typedec.comps.Add(new TypeVal("i32", i.ToString()));
                                            var gv = new Vari("i32*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("i32*", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("i32", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;

                                        }
                                        else if (value.cls == local.Short)
                                        {
                                            typedec.comps.Add(new TypeVal("i16", i.ToString()));
                                            var gv = new Vari("i16*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("i16*", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("i16", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;
                                        }
                                        else if (value.cls == local.Bool)
                                        {
                                            typedec.comps.Add(new TypeVal("i1", i.ToString()));
                                            var gv = new Vari("i1*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("i1*", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("i1", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;
                                        }
                                        else if (value.cls == local.Str)
                                        {
                                            typedec.comps.Add(new TypeVal("%StringType*", i.ToString()));
                                            var gv = new Vari("%StringType*", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("%StringType**", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            var vx0 = new Vari("%StringType*", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx0, vxp));
                                            var utf8 = new Vari("%StringUTF8Type*", "%utf8" + LLVM.n++);
                                            func.comps.Add(new Load(utf8, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                            var len = new Vari("i32*", "%len" + LLVM.n++);
                                            func.comps.Add(new Alloca(len));
                                            vx = new Vari("i8*", "%vx" + LLVM.n++);
                                            func.comps.Add(new Call(vx, utf8, vx0, len));
                                            count += 8;
                                        }
                                        else
                                        {
                                            typedec.comps.Add(new TypeVal("%GCModel*", i.ToString()));
                                            var gv = new Vari("%GCModel**", "%gv" + LLVM.n++);
                                            local.llvm.func.comps.Add(new Gete(typename, gv, objp, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            local.llvm.func.comps.Add(new Store(gv, value.vari));
                                            var vxp = new Vari("%GCModel**", "%vxp" + LLVM.n++);
                                            func.comps.Add(new Gete(typename, vxp, objv, new Vari("i32", "0"), new Vari("i32", n3.ToString())));
                                            vx = new Vari("%GCModel*", "%vx" + LLVM.n++);
                                            func.comps.Add(new Load(vx, vxp));
                                            count += 8;
                                        }
                                        var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                        func.comps.Add(new Call(new Vari("ptr", hv.name), new Vari("ptr", "@DbStore" + model.identity), tx, vx));
                                        pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                        func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                        var thgc = new Vari("%ThreadGCType*", "%thgc");
                                        var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                        func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                        var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                        func.comps.Add(new Load(qv2, qv));
                                        queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                        func.comps.Add(new Store(queueptr2, qv2));
                                        func.comps.Add(new Call(null, pushqueue1, qv2, hv));

                                        parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                        func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                        func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                        n3++;
                                    }
                                    else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数にはオブジェクトクラスかGeneオブジェクトを指定してください");
                                }
                                else return Obj.Error(ObjType.Error, blk.letters[i], "Storeの引数には値を指定してください");
                            }
                            countv.name = count.ToString();
                            {
                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                var sfinp2 = new Vari("i32*", "%sfinp" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", sfinp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                                func.comps.Add(new Load(sfinv, sfinp2));
                                func.comps.Add(new Store(statevalptr2, sfinv));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_end");
                                list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                func.comps.Add(lab10);
                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                                qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                func.comps.Add(new Load(qvn2, qvn));
                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(queueptr2, qvn2));
                                hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                bt = new Vari("%TxFinishType", "%tf");
                                alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                func.comps.Add(new Alloca(alv));
                                func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
                                var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                var txv = new Vari("i8*", "%txv" + LLVM.n++);
                                func.comps.Add(new Load(txv, txp));
                                func.comps.Add(new Call(null, bt, alv, txv));
                                func.comps.Add(new Load(hvn, alv));
                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(qp, qvn2));
                                func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_fin");
                                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                func.comps.Add(lab10);
                                var stateval = new Vari("i32*", "%stateval" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                                var gcr = new Vari("%GC_AddRootType", "%gcr" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(gcr, new Vari("%GC_AddRootType*", "@GC_AddRoot")));
                                var rv = new Vari("%RootNodeType*", "%rv" + LLVM.n++);
                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                local.llvm.func.comps.Add(new Call(rv, gcr, thgc));
                                var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                                local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, rv, new Vari("ptr", func.y.name), objp));
                                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(qv2, qv));
                                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                txp = new Vari("ptr", "%txp" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                txv = new Vari("ptr", "%tx" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(txv, txp));
                                var eq = new Vari("i1", "%eq" + LLVM.n++);
                                local.llvm.func.comps.Add(new Eq(eq, txv, new Vari("ptr", "null")));
                                var l1 = new Lab("eq" + LLVM.n++);
                                var l2 = new Lab("th" + LLVM.n++);
                                local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                local.llvm.func.comps.Add(l1);
                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                local.llvm.func.comps.Add(new Store(statevalptr2, new Vari("i32", "-2")));
                                local.llvm.func.comps.Add(new Br(null, l2));
                                local.llvm.func.comps.Add(l2);

                                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                local.llvm.func.comps.Add(new Store(parent2, hv));
                                //local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                            }
                            return new VoiVal();

                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "Storeの後に()がありません");
                    }
                    else if (word.name == "await")
                    {
                        if (val2.type == ObjType.CallBlock)
                        {

                            if (identity == 0) identity = LLVM.n++;
                            ifv = new Dictionary<string, IfValue>();
                            String type;
                            this.n = Obj.cn++;
                            var callname = "@" + this.letter.name + "ModelCall" + identity;
                            this.vari = new Vari("void", callname);
                            var obj = new Vari("%CoroFrameType*", "%frame");
                            var func = new Func(local.llvm, new Vari("void", callname), obj); ;
                            func.async = true;
                            this.model = "%mmm" + identity;
                            //mmm1(ThreadGC, Handle, RootNode)


                            var rn6 = new Vari("%RootNodeType*", "%rn");
                            var block = new Vari("i8*", "%block");
                            var funcptr = new Vari("i8*", "%fptr");
                            var funcptr2 = new Vari("i8*", "%fptr2");
                            var decname = "@" + this.letter.name + "dec" + identity;
                            var funcdec = new Func(local.llvm, new Vari(this.model + "*", decname), rn6, block);
                            local.llvm.comps.Add(funcdec);
                            var thgcptr6 = new Vari("%ThreadGCType**", "%thgcptr");
                            funcdec.comps.Add(new Gete("%RootNodeType", thgcptr6, rn6, new Vari("i32", "0"), new Vari("i32", "0")));
                            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
                            funcdec.comps.Add(new Load(thgc6, thgcptr6));

                            var root = new Vari("%RootNodeType*", "%rn");
                            var oj = new Vari("%GCObjectPtr", "%oj" + LLVM.n++);
                            var ov = new Vari("%GCObjectPtr", "%ov" + LLVM.n++);
                            if (local.llvm.func.async)
                            {
                                var objf = new Vari("%CoroFrameType*", "%frame");
                                var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", ogv, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                                local.llvm.func.comps.Add(new Load(oj, ogv));
                            }
                            else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock)
                            {
                                local.llvm.func.comps.Add(new Load(oj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                            }
                            else
                            {
                                local.llvm.func.comps.Add(new Load(oj, local.blocks.Last().obj.obj.bas));
                            }
                            local.llvm.func.comps.Add(new Call(ov, funcdec.y, root, oj));
                            var hv = new Vari("%CoroFrameType*", "%fr" + LLVM.n++);
                            var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                            local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                            if (local.llvm.func.async)
                            {
                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                var thgc = new Vari("%ThreadGCType*", "%thgc");
                                var qv = new Vari("%CoroutineQueueType*", "%qv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                                var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(qv2, qv));
                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr2, hv, new Vari("i32", "0"), new Vari("i32", "4")));
                                local.llvm.func.comps.Add(new Store(queueptr2, qv2));
                                local.llvm.func.comps.Add(new Call(null, pushqueue1, qv2, hv));
                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr2, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                var nvari = new Vari("i32", (LLVM.n++).ToString());
                                local.llvm.func.comps.Add(new Store(statevalptr2, nvari));

                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent2, hv, new Vari("i32", "0"), new Vari("i32", "6")));
                                local.llvm.func.comps.Add(new Store(parent2, local.llvm.func.draws[0] as Vari));
                                local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                                var lab6 = new Lab("aw" + LLVM.n++);
                                local.llvm.func.comps.Add(lab6);
                                var gev0 = new Vari(this.model + "**", "%gv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", gev0, local.llvm.func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "5")));
                                var gev02 = new Vari(this.model + "*", "%gv2" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(gev02, gev0));
                                var objptr0 = new Vari(this.model + "**", "%objptr" + LLVM.n++);
                                var alloca0 = new Alloca(objptr0);
                                local.llvm.func.comps.Add(alloca0);
                                var objstore0 = new Store(objptr0, gev02);
                                local.llvm.func.comps.Add(objstore0);
                                local.llvm.func.sc.values.Add(new IfValue(lab6, nvari));
                                //give old-handle;
                                //push handle;
                                //return;
                                //lab
                                //on-final; exe old-handle;
                            }
                            else
                            {
                                var txp = new Vari("ptr", "%txp" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn6, new Vari("i32", "0"), new Vari("i32", "2")));
                                var txv = new Vari("ptr", "%tx" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(txv, txp));
                                var eq = new Vari("i1", "%eq" + LLVM.n++);
                                local.llvm.func.comps.Add(new Eq(eq, txv, new Vari("ptr", "null")));
                                var l1 = new Lab("eq" + LLVM.n++);
                                var l2 = new Lab("th" + LLVM.n++);
                                local.llvm.func.comps.Add(new Br(eq, l1, l2));
                                local.llvm.func.comps.Add(l1);
                                var statevalptr22 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr22, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                                var nvari = new Vari("i32", (LLVM.n++).ToString());
                                local.llvm.func.comps.Add(new Store(statevalptr22, new Vari("i32", "-2")));
                                var waithandle = new Vari("%WaitHandleType", "%waithandle" + LLVM.n++);
                                local.llvm.func.comps.Add(new Br(null, l2));
                                local.llvm.func.comps.Add(l2);
                                local.llvm.func.comps.Add(new Load(waithandle, new Vari("%WaitHandleType*", "@WaitHandle")));
                                var ret = new Vari("i8*", "%ret" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(ret, waithandle, thgc6, hv));
                                this.vari = ret;
                                //make Coroutine Queue;
                                //push handle;
                                //loop
                            }

                            local.llvm.comps.Add(func);
                            local.llvm.funcs.Add(func);
                            //normal; make roots;make handle; push handle;| await; make Coroutine Queue;make handle;push handle;| In Cotoutine[nomarl; make roots;make handle; push handle;| await; make handle; give old-handle; push handle;on-final; exe old-handle;]
                            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                            var rngv = new Vari("%RootNodeType**", "%rngv");
                            func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                            var rn4 = new Vari("%RootNodeType*", "%rn");
                            func.comps.Add(new Load(rn4, rngv));
                            func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                            var geterv = new Vari("i32*", "%state");
                            func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                            var statev = new Vari("i32", "%statev" + LLVM.n++);
                            func.comps.Add(new Load(statev, geterv));
                            var list = new List<IfValue>();
                            var lab10 = new Lab("coro_tx");
                            if (!local.llvm.func.async)
                            {
                                list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                            }
                            var lab11 = new Lab("coro_start");
                            list.Add(new IfValue(lab11, new Vari("i32", "0")));
                            func.sc = new SwitchComp(statev, lab11, list);
                            func.comps.Add(func.sc);

                            if (!local.llvm.func.async)
                            {
                                func.comps.Add(lab10);
                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                                var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                func.comps.Add(new Load(qvn2, qvn));
                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(queueptr2, qvn2));
                                var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                var bt = new Vari("%BeginTransactionType", "%bt");
                                var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                func.comps.Add(new Alloca(alv));
                                func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                                func.comps.Add(new Call(null, bt, alv, thgc4, new Vari("ptr", "@sqlp"), qvn2));
                                func.comps.Add(new Load(hvn, alv));
                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(qp, qvn2));
                                func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "0")));
                                var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                                func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                            }

                            var gmvari = new Vari("%GC_mallocType", "%gm");
                            funcdec.comps.Add(new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc")));
                            var go_v = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                            var tmp = new Vari("i32", this.n.ToString());
                            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
                            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
                            funcdec.comps.Add(new Add(tmv, tmi, tmp));
                            var go_call = new Call(go_v, gmvari, thgc4, tmv);
                            funcdec.comps.Add(go_call);

                            var vc = new Vari("i8*", "%v" + LLVM.n++);
                            var gete = new Gete(this.model, vc, go_v, new Vari("i32", "0"), new Vari("i32", "0"));
                            funcdec.comps.Add(gete);
                            funcdec.comps.Add(new Store(vc, block));

                            func.comps.Add(lab11);
                            var gev = new Vari(this.model + "**", "%gv");
                            func.comps.Add(new Gete("%CoroFrameType", gev, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                            var gev2 = new Vari(this.model + "*", "%gv2");
                            func.comps.Add(new Load(gev2, gev));
                            var objptr = new Vari(this.model + "**", "%objptr");
                            var alloca = new Alloca(objptr);
                            func.comps.Add(alloca);
                            var objstore = new Store(objptr, gev2);
                            func.comps.Add(objstore);
                            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
                            func.comps.Add(srload);
                            var srcall = new Call(null, srv, rn4, objptr);
                            func.comps.Add(srcall);

                            var gev3 = new Vari("i8**", "%gv3");
                            func.comps.Add(new Gete("%CoroFrameType", gev3, obj, new Vari("i32", "0"), new Vari("i32", "8")));
                            var gev4 = new Vari("i8*", "%gv4");
                            func.comps.Add(new Load(gev4, gev3));
                            var txg = new Vari("i8**", "%txg");
                            func.comps.Add(new Gete("%RootNodeType", txg, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                            func.comps.Add(new Store(txg, gev4));

                            var typedec = new TypeDec(this.model);
                            local.llvm.types.Add(typedec);
                            typedec.comps.Add(new TypeVal("i8*", "blk"));

                            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                            var i8p = new Vari("i8*", "%self");
                            var checkname = "@" + this.letter.name + "Check" + identity;
                            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
                            local.llvm.comps.Add(funccheck);
                            var vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
                            funccheck.comps.Add(gete);
                            var co_val = new Vari("%CopyObjectType", "%co");
                            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
                            funccheck.comps.Add(co_load);
                            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            var co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            var co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            var store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);

                            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
                            var ac_val = new Vari("%GC_AddClassType", "%addclass");
                            var countv = new Vari("i32", "0");
                            var name = "await" + LLVM.n++;
                            var strv = new StrV("@" + name, name, name.Length * 1);
                            local.llvm.strs.Add(strv);
                            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
                            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
                            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                            var tv = new Vari("i32", "%cv" + LLVM.n++);
                            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                            var ac_call = new Call(null, ac_val, thgc3, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                            local.llvm.main.comps.Add(ac_call);

                            int count = 8;
                            var blk0 = val2.children[0] as Block;
                            int order = 1;
                            var blk2 = val2.children[1] as Block;
                            var decs = new List<Obj>();

                            foreach (var kv in blk0.vmapA)
                            {
                                var v = kv.Value;
                                if (v.type == ObjType.Variable)
                                {
                                    var variable = v as Variable;
                                    variable.order = order++;
                                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                    if (variable.cls == local.Int)
                                    {
                                        var vdraw = new Vari("i32", "%v" + LLVM.n++);
                                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                                        count += 8;
                                    }
                                    else if (variable.cls == local.Bool)
                                    {
                                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                                        count += 8;
                                    }
                                    else
                                    {
                                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                                        count += 8;

                                        vari = new Vari("i8*", "%v" + LLVM.n++);
                                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                                        funccheck.comps.Add(gete);
                                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                        co_lod = new Load(vari2, vari);
                                        funccheck.comps.Add(co_lod);
                                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                        co_call = new Call(co_cval, co_val, thgc2, vari);
                                        funccheck.comps.Add(co_call);
                                        store = new Store(vari, co_cval);
                                        funccheck.comps.Add(store);
                                    }
                                }
                                else if (v.type == ObjType.Function)
                                {
                                    var f = v as Function;
                                    if (f.identity == 0) f.identity = LLVM.n++;
                                    f.order = order++;
                                    decs.Add(f);
                                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                                    vari = new Vari("i8*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funccheck.comps.Add(gete);
                                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funccheck.comps.Add(gete);
                                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod = new Load(vari2, vari);
                                    funccheck.comps.Add(co_lod);
                                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call = new Call(co_cval, co_val, thgc2, vari);
                                    funccheck.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck.comps.Add(store);
                                    count += 8;

                                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                                    funcdec.comps.Add(fu_call);
                                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                    funcdec.comps.Add(new Load(va2, go_v));
                                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funcdec.comps.Add(gete);
                                    var fu_store = new Store(va, va0);
                                    funcdec.comps.Add(fu_store);

                                }
                                else if (v.type == ObjType.ClassObj)
                                {
                                    var f = v as ClassObj;
                                    if (f.identity == 0) f.identity = LLVM.n++;
                                    f.order = order++;
                                    decs.Add(f);
                                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                                    vari = new Vari("i8*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funccheck.comps.Add(gete);
                                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod = new Load(vari2, vari);
                                    funccheck.comps.Add(co_lod);
                                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call = new Call(co_cval, co_val, thgc2, vari);
                                    funccheck.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck.comps.Add(store);
                                    count += 8;

                                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                                    funcdec.comps.Add(fu_call);
                                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                    funcdec.comps.Add(new Load(va2, go_v));
                                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funcdec.comps.Add(gete);
                                    var fu_store = new Store(va, va0);
                                    funcdec.comps.Add(fu_store);
                                }
                            }
                            foreach (var kv in blk2.vmapA)
                            {
                                var v = kv.Value;
                                if (v.type == ObjType.Variable)
                                {
                                    var variable = v as Variable;
                                    variable.order = order++;
                                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                                    if (variable.cls == local.Int)
                                    {
                                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                                        count += 8;
                                    }
                                    else if (variable.cls == local.Bool)
                                    {
                                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                                        count += 8;
                                    }
                                    else
                                    {
                                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                                        count += 8;

                                        vari = new Vari("i8*", "%v" + LLVM.n++);
                                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                                        funccheck.comps.Add(gete);
                                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                        co_lod = new Load(vari2, vari);
                                        funccheck.comps.Add(co_lod);
                                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                        co_call = new Call(co_cval, co_val, thgc2, vari);
                                        funccheck.comps.Add(co_call);
                                        store = new Store(vari, co_cval);
                                        funccheck.comps.Add(store);
                                    }
                                }
                                else if (v.type == ObjType.Function)
                                {
                                    var f = v as Function;
                                    if (f.identity == 0) f.identity = LLVM.n++;
                                    f.order = order++;
                                    decs.Add(f);
                                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                                    vari = new Vari("i8*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funccheck.comps.Add(gete);
                                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod = new Load(vari2, vari);
                                    funccheck.comps.Add(co_lod);
                                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call = new Call(co_cval, co_val, thgc2, vari);
                                    funccheck.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck.comps.Add(store);
                                    count += 8;

                                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                    funcdec.comps.Add(fu_call);
                                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                    funcdec.comps.Add(new Load(va2, go_v));
                                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funcdec.comps.Add(gete);
                                    var fu_store = new Store(va, va0);
                                    funcdec.comps.Add(fu_store);

                                }
                                else if (v.type == ObjType.ClassObj)
                                {
                                    var f = v as ClassObj;
                                    if (f.identity == 0) f.identity = LLVM.n++;
                                    decs.Add(f);
                                    f.order = order++;
                                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                                    vari = new Vari("i8*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funccheck.comps.Add(gete);
                                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                                    co_lod = new Load(vari2, vari);
                                    funccheck.comps.Add(co_lod);
                                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                                    co_call = new Call(co_cval, co_val, thgc2, vari);
                                    funccheck.comps.Add(co_call);
                                    store = new Store(vari, co_cval);
                                    funccheck.comps.Add(store);
                                    count += 8;

                                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                                    funcdec.comps.Add(fu_call);
                                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                                    funcdec.comps.Add(new Load(va2, go_v));
                                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                                    funcdec.comps.Add(gete);
                                    var fu_store = new Store(va, va0);
                                    funcdec.comps.Add(fu_store);
                                }
                            }
                            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                            funcdec.comps.Add(new Ret(go_v));
                            funccheck.comps.Add(new Ret(new Vari("void", null)));
                            countv.name = count.ToString();
                            this.bas = objptr;
                            local.blocks.Add(blk0);
                            blk0.exeC(local);
                            local.blocks.Add(blk2);

                            local.labs.Add(new Lab("entry"));
                            blk2.exeC(local);
                            local.labs.RemoveAt(local.labs.Count - 1);
                            local.blocks.RemoveAt(local.blocks.Count - 1);
                            local.blocks.RemoveAt(local.blocks.Count - 1);

                            var rnpv = new Vari("i32*", "%ptr");
                            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                            var rnpv12 = new Vari("i32", "%v");
                            rnpv12 = new Vari("i32", "%v");
                            var rnp12load = new Load(rnpv12, rnpv);
                            local.llvm.func.comps.Add(rnp12load);
                            var rnpv2 = new Vari("i32", "%dec");
                            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                            local.llvm.func.comps.Add(rnpsub);
                            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                            if (blk2.vari != null)
                            {
                                var retval = new Vari("i8*", "%ret");
                                var rgete = new Gete("%CoroFrameType", retval, obj, new Vari("i32", "0"), new Vari("i32", "8"));
                                local.llvm.func.comps.Add(rgete);
                                local.llvm.func.comps.Add(new Store(retval, blk2.vari));
                            }
                            if (local.llvm.func.async)
                            {
                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_fin");
                                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                func.comps.Add(lab10);
                            }
                            else {
                                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-3")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_end");
                                list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                                func.comps.Add(lab10);
                                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                                var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                                func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                                var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                                func.comps.Add(new Load(qvn2, qvn));
                                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(queueptr2, qvn2));
                                var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                                var bt = new Vari("%TxFinishType", "%tf");
                                var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                                func.comps.Add(new Alloca(alv));
                                func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
                                var txp = new Vari("i8**", "%txp" + LLVM.n++);
                                func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                                var txv = new Vari("i8*", "%txv" + LLVM.n++);
                                func.comps.Add(new Load(txv, txp));
                                func.comps.Add(new Call(null, bt, alv, txv));
                                func.comps.Add(new Load(hvn, alv));
                                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                                var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                                func.comps.Add(new Store(qp, qvn2));
                                func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                                func.comps.Add(new Ret(new Vari("void", null)));
                                lab10 = new Lab("coro_fin");
                                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                                func.comps.Add(lab10);
                            }
                            var stateval = new Vari("i32*", "%state" + LLVM.n++);
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                            local.llvm.func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                            var parent = new Vari("%CoroFrameType**", "%parent");
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", parent, obj, new Vari("i32", "0"), new Vari("i32", "6")));
                            var handle = new Vari("%CoroFrameType*", "%frame" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(handle, parent));
                            var comp = new Vari("i1", "%comp");
                            local.llvm.func.comps.Add(new Ne(comp, handle, new Vari("ptr", "null")));
                            var lab = new Lab("coro_end" + identity);
                            var lab2 = new Lab("coro_ret");
                            local.llvm.func.comps.Add(new Br(comp, lab, lab2));
                            local.llvm.func.comps.Add(lab);
                            var queueptr = new Vari("%CoroutineQueueType**", "%queueptr");
                            local.llvm.func.comps.Add(new Gete("%CoroFrameType", queueptr, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                            var queue = new Vari("%PushQueueType*", "%queue");
                            local.llvm.func.comps.Add(new Load(queue, queueptr));
                            var pushqueue = new Vari("%PushQueueType*", "%pushqueue");
                            local.llvm.func.comps.Add(new Load(pushqueue, new Vari("%PushQueueType*", "@PushQueue")));
                            local.llvm.func.comps.Add(new Call(null, pushqueue, queue, handle));
                            local.llvm.func.comps.Add(new Br(null, lab2));
                            local.llvm.func.comps.Add(lab2);
                            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
                            n++;
                            return new VoiVal();
                        }
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "modelには" + word.name + "は宣言されていません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の次には名前が来ます");
            }
            else if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                val2 = primary.children[n];
                var gj = local.getC(word.name, local) as ModelObj;
                if (val2.type == ObjType.CallBlock)
                {
                    n++;
                    return gj;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "modelの内容を{}で宣言してください。");
            }
            else if (val2.type == ObjType.CallBlock)
            {
                if (identity == 0) identity = LLVM.n++;
                ifv = new Dictionary<string, IfValue>();
                String type;
                var callname = "@" + this.letter.name + "ModelCall" + identity;
                this.vari = new Vari("void", callname);
                var obj = new Vari("%CoroFrameType*", "%frame");
                var func = new Func(local.llvm, this.vari, obj);
                func.async = true;
                this.model = "%mmm" + identity;

                var rn6 = new Vari("%RootNodeType*", "%rn");
                var block = new Vari("i8*", "%block");
                var funcptr = new Vari("i8*", "%fptr");
                var funcptr2 = new Vari("i8*", "%fptr2");
                var decname = "@" + this.letter.name + "dec" + identity;
                var funcdec = new Func(local.llvm, new Vari(this.model + "*", decname), rn6, block);
                local.llvm.comps.Add(funcdec);
                var thgcptr6 = new Vari("%ThreadGCType**", "%thgcptr");
                funcdec.comps.Add(new Gete("%RootNodeType", thgcptr6, rn6, new Vari("i32", "0"), new Vari("i32", "0")));
                var thgc6 = new Vari("%ThreadGCType*", "%thgc");
                funcdec.comps.Add(new Load(thgc6, thgcptr6));

                var root = new Vari("%RootNodeType*", "%rn");
                var oj = new Vari("%GCObjectPtr", "%oj" + LLVM.n++);
                var ov = new Vari("%GCObjectPtr", "%ov" + LLVM.n++);
                if (local.llvm.func.async)
                {
                    var objf = new Vari("%CoroFrameType*", "%frame");
                    var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                    local.llvm.func.comps.Add(new Gete("%CoroFrameType", ogv, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                    local.llvm.func.comps.Add(new Load(oj, ogv));
                }
                else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock)
                {
                    local.llvm.func.comps.Add(new Load(oj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
                }
                else
                {
                    local.llvm.func.comps.Add(new Load(oj, local.blocks.Last().obj.obj.bas));
                }
                local.llvm.func.comps.Add(new Call(ov, funcdec.y, root, oj));
                //Make root node
                var hv = new Vari("%CoroFrameType*", "%hv");
                var mkf = new Vari("%MakeFrameType", "%mkf" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(mkf, new Vari("%MakeFrameType*", "@MakeFrame")));
                local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), mkf, root, new Vari("ptr", func.y.name), ov));
                var statevalptr22 = new Vari("i32*", "%stateptr" + LLVM.n++);
                var txp = new Vari("ptr", "%txp" + LLVM.n++);
                local.llvm.func.comps.Add(new Gete("%RootNodeType", txp, rn6, new Vari("i32", "0"), new Vari("i32", "2")));
                var txv = new Vari("ptr", "%tx" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(txv, txp));
                var eq = new Vari("i1", "%eq" + LLVM.n++);
                local.llvm.func.comps.Add(new Eq(eq, txv, new Vari("ptr", "null")));
                var l1 = new Lab("eq" + LLVM.n++);
                var l2 = new Lab("th" + LLVM.n++);
                local.llvm.func.comps.Add(new Br(eq, l1, l2));
                local.llvm.func.comps.Add(l1);
                local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr22, hv, new Vari("i32", "0"), new Vari("i32", "7")));
                var nvari = new Vari("i32", (LLVM.n++).ToString());
                local.llvm.func.comps.Add(new Store(statevalptr22, new Vari("i32", "-2")));
                local.llvm.func.comps.Add(new Br(null, l2));
                local.llvm.func.comps.Add(l2);
                var gcr = new Vari("%GC_AddRootType", "%gcr" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(gcr, new Vari("%GC_AddRootType*", "@GC_AddRoot")));
                var rv = new Vari("%RootNodeType*", "%rv" + LLVM.n++);
                var thgc = new Vari("%ThreadGCType*", "%thgc");
                local.llvm.func.comps.Add(new Call(rv, gcr, thgc));
                var rvp = new Vari("%RootNodeType**", "%rvp" + LLVM.n++);
                local.llvm.func.comps.Add(new Gete("%CoroFrameType", rvp, hv, new Vari("i32", "0"), new Vari("i32", "3")));
                local.llvm.func.comps.Add(new Store(rvp, rv));
                //make roots
                //push handle
                var phv = new Vari("%PushQueueType", "%phv" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(phv, new Vari("%PushQueueType*", "@PushQueue")));
                var qv = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(qv2, qv));
                local.llvm.func.comps.Add(new Call(null, phv, qv2, hv));

                local.llvm.comps.Add(func);
                local.llvm.funcs.Add(func);
                //normal; make roots;make handle; push handle;| await; make Coroutine Queue;make handle;push handle;| In Cotoutine[nomarl; make roots;make handle; push handle;| await; make handle; give old-handle; push handle;on-final; exe old-handle;]
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                var rngv = new Vari("%RootNodeType**", "%rngv");
                func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
                var rn4 = new Vari("%RootNodeType*", "%rn");
                func.comps.Add(new Load(rn4, rngv));
                func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

                var geterv = new Vari("i32*", "%state");
                func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                var statev = new Vari("i32", "%statev" + LLVM.n++);
                func.comps.Add(new Load(statev, geterv));
                var list = new List<IfValue>();
                var lab10 = new Lab("coro_tx");
                list.Add(new IfValue(lab10, new Vari("i32", "-2")));
                var lab11 = new Lab("coro_start");
                list.Add(new IfValue(lab11, new Vari("i32", "0")));
                func.sc = new SwitchComp(statev, lab11, list);
                func.comps.Add(func.sc);

                var gmvari = new Vari("%GC_mallocType", "%gm");
                funcdec.comps.Add(new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc")));
                var go_v = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                var tmp = new Vari("i32", this.n.ToString());
                var tmi = new Vari("i32", "%tmi" + LLVM.n++);
                funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
                var tmv = new Vari("i32", "%tmv" + LLVM.n++);
                funcdec.comps.Add(new Add(tmv, tmi, tmp));
                var go_call = new Call(go_v, gmvari, thgc4, tmv);
                funcdec.comps.Add(go_call);

                var vc = new Vari("i8*", "%v" + LLVM.n++);
                var gete = new Gete(this.model, vc, go_v, new Vari("i32", "0"), new Vari("i32", "0"));
                funcdec.comps.Add(gete);
                funcdec.comps.Add(new Store(vc, block));

                func.comps.Add(lab10);
                var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                func.comps.Add(new Gete("%ThreadGCType", qvn, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
                var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                func.comps.Add(new Load(qvn2, qvn));
                var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                func.comps.Add(new Store(queueptr2, qvn2));
                var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                var bt = new Vari("%BeginTransactionType", "%bt");
                var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                func.comps.Add(new Alloca(alv));
                func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
                func.comps.Add(new Call(null, bt, alv, thgc, new Vari("ptr", "@sqlp"), qvn2));
                func.comps.Add(new Load(hvn, alv));
                var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                func.comps.Add(new Store(qp, qvn2));
                func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                func.comps.Add(new Store(statevalptr2, new Vari("i32", "0")));
                var sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                func.comps.Add(new Store(sfinp, new Vari("i32", "-3")));
                func.comps.Add(new Ret(new Vari("void", null)));

                func.comps.Add(lab11);
                var txp2 = new Vari("i8**", "%txp" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", txp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
                var tx = new Vari("i8*", "%txv" + LLVM.n++);
                func.comps.Add(new Load(tx, txp2));
                var txptr = new Vari("i8**", "%tx" + LLVM.n++);
                func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                func.comps.Add(new Store(txptr, tx));
                var gev = new Vari(this.model + "**", "%gv");
                func.comps.Add(new Gete("%CoroFrameType", gev, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                var gev2 = new Vari(this.model + "*", "%gv2");
                func.comps.Add(new Load(gev2, gev));
                var objptr = new Vari(this.model + "**", "%objptr");
                var alloca = new Alloca(objptr);
                func.comps.Add(alloca);
                var objstore = new Store(objptr, gev2);
                func.comps.Add(objstore);
                var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
                var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
                func.comps.Add(srload);
                var srcall = new Call(null, srv, rn4, objptr);
                func.comps.Add(srcall);

                var typedec = new TypeDec(this.model);
                local.llvm.types.Add(typedec);
                typedec.comps.Add(new TypeVal("i8*", "blk"));

                var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                var i8p = new Vari("i8*", "%self");
                var checkname = "@" + this.letter.name + "Check" + identity;
                var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
                local.llvm.comps.Add(funccheck);
                var vari = new Vari("i8*", "%v" + LLVM.n++);
                gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
                funccheck.comps.Add(gete);
                var co_val = new Vari("%CopyObjectType", "%co");
                var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
                funccheck.comps.Add(co_load);
                var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                var co_lod = new Load(vari2, vari);
                funccheck.comps.Add(co_lod);
                var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                var co_call = new Call(co_cval, co_val, thgc2, vari);
                funccheck.comps.Add(co_call);
                var store = new Store(vari, co_cval);
                funccheck.comps.Add(store);

                var thgc3 = new Vari("%ThreadGCType*", "%thgc");
                var ac_val = new Vari("%GC_AddClassType", "%addclass");
                var countv = new Vari("i32", "0");
                var name = "async" + LLVM.n++;
                var strv = new StrV("@" + name, name, name.Length * 1);
                local.llvm.strs.Add(strv);
                var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
                var tnp = new Vari("i32", "%tnp" + LLVM.n++);
                local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
                var tv = new Vari("i32", "%cv" + LLVM.n++);
                local.llvm.main.comps.Add(new Add(tv, tnp, typ));
                var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
                local.llvm.main.comps.Add(ac_call);

                int count = 8;
                var blk0 = val2.children[0] as Block;
                int order = 1;
                var blk2 = val2.children[1] as Block;
                var decs = new List<Obj>();

                foreach (var kv in blk0.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            var vdraw = new Vari("i32", "%v" + LLVM.n++);
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            var vdraw = new Vari("i1", "%v" + LLVM.n++);
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            count += 8;

                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            funccheck.comps.Add(gete);
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, go_v));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, go_v));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            count += 8;

                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            funccheck.comps.Add(gete);
                            vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, go_v));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        decs.Add(f);
                        f.order = order++;
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, go_v));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);
                    }
                }
                for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                funcdec.comps.Add(new Ret(go_v));
                funccheck.comps.Add(new Ret(new Vari("void", null)));
                countv.name = count.ToString();
                this.bas = objptr;
                local.blocks.Add(blk0);
                blk0.exeC(local);
                local.blocks.Add(blk2);

                local.labs.Add(new Lab("entry"));
                blk2.exeC(local);
                local.labs.RemoveAt(local.labs.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);
                
                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                sfinp = new Vari("i32*", "%sfinp" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", sfinp, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "10")));
                var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
                func.comps.Add(new Load(sfinp, sfinv));
                func.comps.Add(new Store(statevalptr2, sfinv));
                func.comps.Add(new Ret(new Vari("void", null)));
                lab10 = new Lab("coro_end");
                list.Add(new IfValue(lab10, new Vari("i32", "-3")));
                func.comps.Add(lab10);
                pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
                func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
                qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
                func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
                qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
                func.comps.Add(new Load(qvn2, qvn));
                queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
                func.comps.Add(new Store(queueptr2, qvn2));
                hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
                bt = new Vari("%TxFinishType", "%tf");
                alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
                func.comps.Add(new Alloca(alv));
                func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
                txp = new Vari("i8**", "%txp" + LLVM.n++);
                func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
                txv = new Vari("i8*", "%txv" + LLVM.n++);
                func.comps.Add(new Load(txv, txp));
                func.comps.Add(new Call(null, bt, alv, txv));
                func.comps.Add(new Load(hvn, alv));
                parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
                func.comps.Add(new Store(parent2, func.draws[0] as Vari));
                qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
                func.comps.Add(new Store(qp, qvn2));
                func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
                statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
                func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
                func.comps.Add(new Store(statevalptr2, new Vari("i32", "-1")));
                func.comps.Add(new Ret(new Vari("void", null)));
                lab10 = new Lab("coro_fin");
                list.Add(new IfValue(lab10, new Vari("i32", "-1")));
                func.comps.Add(lab10);

                var rnpv = new Vari("i32*", "%ptr");
                local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
                var rnpv12 = new Vari("i32", "%v");
                rnpv12 = new Vari("i32", "%v");
                var rnp12load = new Load(rnpv12, rnpv);
                local.llvm.func.comps.Add(rnp12load);
                var rnpv2 = new Vari("i32", "%dec");
                var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
                local.llvm.func.comps.Add(rnpsub);
                local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
                /*var retval = new Vari("i8*", "%ret");
                var rgete = new Gete("%CoroFrameType", retval, obj, new Vari("i32", "0"), new Vari("i32", "5"));
                local.llvm.func.comps.Add(rgete);
                local.llvm.func.comps.Add(new Store(retval, blk2.vari));*/
                var stateval = new Vari("i32*", "%state" + LLVM.n++);
                local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
                local.llvm.func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
                local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
                local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
                n++;
                return new VoiVal();
            }
            return Obj.Error(ObjType.NG, val2.letter, "model名を宣言してください。");
        }
    }
    partial class ModelObj
    {
        public override string model
        {
            get
            {
                if (initial) return _model;
                else return base.model;
            }
            set => _model = value;
        }
        public override Obj exeC(Local local)
        {
            return this;
        }
        public override async void Dec(Local local)
        {
            if (identity == 0) identity = LLVM.n++;
            local.migrate.AddModel(letter.name, this, local);
            ifv = new Dictionary<string, IfValue>();
            this.n = Obj.cn++;
            this.vari = new Vari("void", call);
            var rn4 = new Vari("%RootNodeType*", "%rn");
            var obj = new Vari(this.model + "*", "%obj");
            var func = new Func(local.llvm, this.vari, rn4, obj);
            local.llvm.comps.Add(func);
            local.llvm.funcs.Add(func);
            var thgcptr4 = new Vari("%ThreadGCType**", "%thgcptr");
            func.comps.Add(new Gete("%RootNodeType", thgcptr4, rn4, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            func.comps.Add(new Load(thgc4, thgcptr4));

            var objptr = new Vari(this.model + "**", "%objptr");
            var alloca = new Alloca(objptr);
            func.comps.Add(alloca);
            var objstore = new Store(objptr, obj);
            func.comps.Add(objstore);
            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            func.comps.Add(srload);
            var srcall = new Call(null, srv, rn4, objptr);
            func.comps.Add(srcall);


            var rn5 = new Vari("%RootNodeType*", "%rn");
            var blk = new Vari("i8*", "%fptr1");
            var funcdraw = new Func(local.llvm, new Vari(this.model + "*", drawcall), rn5, blk);
            local.llvm.comps.Add(funcdraw);

            var typedec = new TypeDec(this.model);
            local.llvm.types.Add(typedec);
            typedec.comps.Add(new TypeVal("i8*", "blk"));

            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var checkname = "@" + this.letter.name + "Check" + identity;
            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            var countv = new Vari("i32", "0");
            var name = "for" + LLVM.n++;
            var strv = new StrV("@" + name, name, name.Length * 1);
            local.llvm.strs.Add(strv);
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var thgcptr5 = new Vari("%ThreadGCType**", "%thgcptr");
            funcdraw.comps.Add(new Gete("%RootNodeType", thgcptr5, rn5, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc5 = new Vari("%ThreadGCType*", "%thgc");
            funcdraw.comps.Add(new Load(thgc5, thgcptr5));
            var go_val = new Vari("%CopyObjectType", "%gcobject");
            var go_load = new Load(go_val, new Vari("%CopyObjectType*", "@CloneObject"));
            funcdraw.comps.Add(go_load);
            var v3 = new Vari("i8**", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", v3, blk, new Vari("i32", "0"), new Vari("i32", "3"));
            funcdraw.comps.Add(gete);
            var v4 = new Vari("i8*", "%v" + LLVM.n++);
            funcdraw.comps.Add(new Load(v4, v3));
            var go_v = new Vari("i8*", "%obj");
            var go_call = new Call(go_v, go_val, thgc5, v4);
            funcdraw.comps.Add(go_call);
            var go_c = new Vari(this.model + "*", "%v" + LLVM.n++);
            var go_cast = new Bitcast(go_c, go_v);
            funcdraw.comps.Add(go_cast);

            var rn6 = new Vari("%RootNodeType*", "%rn");
            var block = new Vari("i8*", "%block");
            var funcptr = new Vari("i8*", "%fptr");
            var funcptr2 = new Vari("i8*", "%fptr2");
            var decname = "@" + this.letter.name + "dec" + identity;
            var funcdec = new Func(local.llvm, new Vari("%FuncType*", decname), rn6, block, funcptr, funcptr2);
            local.llvm.comps.Add(funcdec);
            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
            funcdec.comps.Add(new Load(thgc6, new Vari("%ThreadGCType**", "@thgcp")));

            var gmvari = new Vari("%GC_mallocType", "%gm");
            var gmload = new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc"));
            funcdec.comps.Add(gmload);
            var tmp = new Vari("i32", 28.ToString());
            go_call = new Call(go_v, gmvari, thgc6, tmp);
            funcdec.comps.Add(go_call);
            var go_c3 = new Vari("%FuncType*", "%v" + LLVM.n++);
            go_cast = new Bitcast(go_c3, go_v);
            funcdec.comps.Add(go_cast);

            var vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc, go_c3, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);
            var vc2 = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc2, go_c3, new Vari("i32", "0"), new Vari("i32", "1"));
            funcdec.comps.Add(gete);
            store = new Store(vc2, funcptr);
            funcdec.comps.Add(store);
            var vc3 = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc3, go_c3, new Vari("i32", "0"), new Vari("i32", "2"));
            funcdec.comps.Add(gete);
            store = new Store(vc3, funcptr2);
            funcdec.comps.Add(store);

            var objptr2 = new Vari("%FuncType**", "%objptr");
            var alloca2 = new Alloca(objptr2);
            funcdec.comps.Add(alloca2);
            var objstore2 = new Store(objptr2, go_c3);
            funcdec.comps.Add(objstore2);

            srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            funcdec.comps.Add(srload);
            srcall = new Call(null, srv, rn6, objptr2);
            funcdec.comps.Add(srcall);


            go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
            tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            funcdec.comps.Add(new Add(tmv, tmi, tmp));
            go_call = new Call(go_v, gmvari, thgc6, tmv);
            funcdec.comps.Add(go_call);
            var go_c2 = new Vari(this.model + "*", "%v" + LLVM.n++);
            go_cast = new Bitcast(go_c2, go_v);
            funcdec.comps.Add(go_cast);

            vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete(this.model, vc, go_c2, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);

            var objptr3 = new Vari(this.model + "**", "%v" + LLVM.n++);
            var alloca3 = new Alloca(objptr3);
            funcdec.comps.Add(alloca3);
            var objstore3 = new Store(objptr3, go_c2);
            funcdec.comps.Add(objstore3);

            srcall = new Call(null, srv, rn6, objptr3);
            funcdec.comps.Add(srcall);

            var vc4 = new Vari(this.model + "**", "%v" + LLVM.n++);
            gete = new Gete("%FuncType", vc4, go_c3, new Vari("i32", "0"), new Vari("i32", "3"));
            funcdec.comps.Add(gete);
            var parentstore = new Store(vc4, go_c2);
            funcdec.comps.Add(parentstore);

            int count = 8;
            var blk0 = draw.children[0] as Block;
            int order = 2;
            var blk2 = draw.children[1] as Block;
            var decs = new List<Obj>();
            typedec.comps.Add(new TypeVal("i32", "id"));
            foreach (var kv in blk0.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        var vdraw = new Vari("i32", "%v" + LLVM.n++);
                        funcdraw.draws.Add(vdraw);
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;

                        var varii = new Vari("i32*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);
                    }
                    else if (variable.cls == local.Bool)
                    {
                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                        funcdraw.draws.Add(new Vari("i32", "%v" + LLVM.n++));
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;

                        var varii = new Vari("i1*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        funcdraw.draws.Add(vdraw);
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        var varii = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        var geteinit = new Gete(this.model, varii, go_c, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funcdraw.comps.Add(geteinit);
                        var vstore = new Store(varii, vdraw);
                        funcdraw.comps.Add(vstore);

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    funcdraw.draws.Add(new Vari(f.model + "*", "%v" + LLVM.n++));
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj || v.type == ObjType.ModelObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));
                    funcdraw.draws.Add(new Vari("%FuncType*", "%v" + LLVM.n++));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);
                }
            }
            foreach (var kv in blk2.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    if (kv.Key == "id")
                    {
                        variable.order = 1;
                        count += 8;
                        continue;
                    }
                    else variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj || v.type == ObjType.ModelObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    decs.Add(f);
                    f.order = order++;
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                    va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdraw.comps.Add(fu_call);
                    va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, go_c, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdraw.comps.Add(gete);
                    fu_store = new Store(va, va0);
                    funcdraw.comps.Add(fu_store);
                }
            }
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            local.llvm.funcs.Add(funcdec);
            this.bas = objptr2;
            local.blocks.Add(blk0);
            blk0.exeC(local);
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            local.blocks.Add(blk2);

            funcdraw.comps.Add(new Ret(go_c));
            countv.name = count.ToString();
            funccheck.comps.Add(new Ret(new Vari("void", null)));
            var vv = new Vari("%FuncType*", "%v" + LLVM.n++);
            funcdec.comps.Add(new Load(vv, objptr2));

            var rnpv = new Vari("i32*", "%ptr");
            funcdec.comps.Add(new Gete("%RootNodeType", rnpv, rn6, new Vari("i32", "0"), new Vari("i32", "1")));
            var rnpv12 = new Vari("i32", "%v");
            var rnp12load = new Load(rnpv12, rnpv);
            funcdec.comps.Add(rnp12load);
            var rnpv2 = new Vari("i32", "%dec");
            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            funcdec.comps.Add(rnpsub);
            funcdec.comps.Add(new Store(rnpv, rnpv2));
            funcdec.comps.Add(new Ret(go_c3));

            this.bas = objptr;
            local.labs.Add(new Lab("entry"));
            blk2.exeC(local);
            local.labs.RemoveAt(local.labs.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);

            rnpv = new Vari("i32*", "%ptr");
            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
            rnpv12 = new Vari("i32", "%v");
            rnp12load = new Load(rnpv12, rnpv);
            local.llvm.func.comps.Add(rnp12load);
            rnpv2 = new Vari("i32", "%dec");
            rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            local.llvm.func.comps.Add(rnpsub);
            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));

            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
        }
        bool decstore;
        public String TableName()
        {
            var tname = rename + "_";
            int n = 0;
            var dot = false;
            for(int i = 0; i < version.Length; i++)
            {
                if ('0' <= version[i] && version[i] <= '9')
                {
                    tname += version[i];
                }
                else if (version[i] == '.')
                {
                    tname += "_";
                }
            }
            if (!dot) tname += "_0";
            return tname;
        }
        public void Store(Local local)
        {
            if (!decstore)
            {
                var tx = new Vari("i8*", "%tx");
                var obj = new Vari("%GCModel*", "%obj");
                var storefunc = new Func(local.llvm, new Vari("ptr", "@DbStore" + this.identity), tx, obj);//async await tx data
                local.llvm.comps.Add(storefunc);
                //if (id == 0) Insert()
                //else Upsert()
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                storefunc.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));
                var idp = new Vari("i32*", "%idp");
                storefunc.comps.Add(new Gete("%GCModel", idp, obj, new Vari("i32", "0"), new Vari("i32", "1")));
                var idv = new Vari("i32", "%idv");
                storefunc.comps.Add(new Load(idv, idp));
                var compv = new Vari("i1", "%compv");
                storefunc.comps.Add(new Eq(compv, idv, new Vari("i32", "0")));
                var lab1 = new Lab("insert");
                var lab2 = new Lab("upsert");
                storefunc.comps.Add(new Br(compv, lab1, lab2));
                storefunc.comps.Add(lab1);
                String na = TableName();
                var upsert = "Insert Into " + na + "(id,";
                var insert = "Insert Into " + na + "(";
                var values = "values (";
                var uvalues = "values (";
                var uvaluesafter = "";
                var conflicts = "on conflict(id) do update set";
                var where = " where ";
                int n = 0;
                var blk1 = draw.children[0] as Block;
                foreach (var kv in blk1.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (n != 0)
                            {
                                insert += ", ";
                                values += ", ";
                                upsert += ", ";
                                uvaluesafter += ", ";
                                conflicts += ", ";
                                where += " or ";
                            }
                            insert += kv.Key;
                            values += "?" + n;
                            upsert += kv.Key;
                            uvaluesafter += "?" + n;
                            conflicts += " " + kv.Key + " = excluded." + kv.Key;
                            where += na + "." + kv.Key + " is not excluded." + kv.Key;
                            n++;
                        }
                    }
                }
                var blk2 = draw.children[1] as Block;
                foreach (var kv in blk2.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        if (kv.Key == "id") continue;
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (n != 0)
                            {
                                insert += ", ";
                                values += ", ";
                                upsert += ", ";
                                uvaluesafter += ", ";
                                conflicts += ", ";
                                where += " or ";
                            }
                            insert += kv.Key;
                            values += "?" + (n + 1);
                            upsert += kv.Key;
                            uvaluesafter += "?" + (n + 1);
                            conflicts += " " + kv.Key + " = excluded." + kv.Key;
                            where += na + "." + kv.Key + " is not excluded." + kv.Key;
                            n++;
                        }
                    }
                }
                insert += ")";
                values += ")";
                insert += values + ";";
                var ps = new Vari("ptr", "%ps");
                storefunc.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * n).ToString())));//malloc
                var n2 = 0;
                foreach (var kv in blk1.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (variable.cls == local.Int)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("i32*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("i32", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                storefunc.comps.Add(new Store(vptr, ov));
                            }
                            else if (variable.cls == local.Str)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("%StringType*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                //StringToUTF8
                                var utf8f = new Vari("%StringUTF8", "%utf8f");
                                storefunc.comps.Add(new Load(utf8f, new Vari("%StringUTF8*", "@StringUTF8")));
                                var uc = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Alloca(uc));
                                var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Call(utf8v, utf8f, ov, uc));
                                storefunc.comps.Add(new Store(vptr, utf8v));
                                var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                var uv = new Vari("i32", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Load(uv, uc));
                                storefunc.comps.Add(new Store(lptr, uv));
                            }
                            n2++;
                        }
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        if (kv.Key == "id")
                        {
                            uvalues += "?" + n;
                            if (n != 0) uvalues += ",";
                            continue;
                        }
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (variable.cls == local.Int)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("i32*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("i32", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                storefunc.comps.Add(new Store(vptr, ov));
                            }
                            else if (variable.cls == local.Str)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("%StringType*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                //StringToUTF8
                                var utf8f = new Vari("%StringUTF8Type", "%utf8f");
                                storefunc.comps.Add(new Load(utf8f, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                var uc = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Alloca(uc));
                                var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Call(utf8v, utf8f, ov, uc));
                                storefunc.comps.Add(new Store(vptr, utf8v));
                                var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                var uv = new Vari("i32", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Load(uv, uc));
                                storefunc.comps.Add(new Store(lptr, uv));
                            }
                            n2++;
                        }
                    }
                }
                var sv = new StrV("@s" + LLVM.n++, insert, insert.Length);
                local.llvm.strs.Add(sv);
                var v2 = new Vari("i8*", "%ins");
                storefunc.comps.Add(new Gete("[" + (insert.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                storefunc.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                var v = new Vari("%StringType*", "%str" + LLVM.n++);
                storefunc.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", (insert.Length * 1).ToString()), new Vari("i32", 1.ToString())));
                var esql = new Vari("%ExecSqlType", "%esql");
                storefunc.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                var v3 = new Vari("%StringType*", "%v" + LLVM.n++);
                var coroptr = new Vari("%CoroFrameType**", "%coroptr");
                storefunc.comps.Add(new Alloca(coroptr));
                storefunc.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", n.ToString()), new Vari("ptr", "null"), new Vari("ptr", "null")));
                var corov = new Vari("%CoroFrameType*", "%corov");
                storefunc.comps.Add(new Load(corov, coroptr));
                storefunc.comps.Add(new Ret(corov));
                storefunc.comps.Add(lab2);
                ps = new Vari("ptr", "%psa");
                storefunc.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", (24 * (n + 1)).ToString())));//malloc
                n2 = 0;
                foreach (var kv in blk1.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (variable.cls == local.Int)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("i32*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("i32", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                storefunc.comps.Add(new Store(vptr, ov));
                            }
                            else if (variable.cls == local.Str)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("%StringType*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                //StringToUTF8
                                var utf8f = new Vari("%StringUTF8", "%utf8f");
                                storefunc.comps.Add(new Load(utf8f, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                var uc = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Alloca(uc));
                                var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Call(utf8v, utf8f, ov, uc));
                                storefunc.comps.Add(new Store(vptr, utf8v));
                                var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                var uv = new Vari("i32", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Load(uv, uc));
                                storefunc.comps.Add(new Store(lptr, uv));
                            }
                            n2++;
                        }
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (kv.Key == "id")
                        {
                            var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                            storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n.ToString())));
                            var kptr = new Vari("i32*", "%k" + LLVM.n++);
                            storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                            storefunc.comps.Add(new Store(kptr, new Vari("i32", "1")));
                            var vptr = new Vari("i32*", "%v" + LLVM.n++);
                            storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                            var ovp = new Vari("i32*", "%ov" + LLVM.n++);
                            storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                            var ov = new Vari("i32", "%ov" + LLVM.n++);
                            storefunc.comps.Add(new Load(ov, ovp));
                            storefunc.comps.Add(new Store(vptr, ov));
                        }
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (variable.cls == local.Int)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "1")));
                                var vptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                                var ovp = new Vari("i32*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("i32", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                storefunc.comps.Add(new Store(vptr, ov));
                            }
                            else if (variable.cls == local.Str)
                            {
                                var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                                storefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n2.ToString())));
                                var kptr = new Vari("i32*", "%k" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                                storefunc.comps.Add(new Store(kptr, new Vari("i32", "4")));
                                var vptr = new Vari("%TextType*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                var ovp = new Vari("%StringType**", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                var ov = new Vari("%StringType*", "%ov" + LLVM.n++);
                                storefunc.comps.Add(new Load(ov, ovp));
                                //StringToUTF8
                                var utf8f = new Vari("%StringUTF8", "%utf8f");
                                storefunc.comps.Add(new Load(utf8f, new Vari("%StringUTF8Type*", "@StringUTF8")));
                                var uc = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Alloca(uc));
                                var utf8v = new Vari("i8*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Call(utf8v, utf8f, ov, uc));
                                storefunc.comps.Add(new Store(vptr, utf8v));
                                var lptr = new Vari("i32*", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Gete("%TextType", lptr, vptr, new Vari("i32", "0"), new Vari("i32", "1")));
                                var uv = new Vari("i32", "%v" + LLVM.n++);
                                storefunc.comps.Add(new Load(uv, uc));
                                storefunc.comps.Add(new Store(lptr, uv));
                            }
                            n2++;
                        }
                    }
                }
                upsert += ")";
                uvaluesafter += ")";
                upsert += uvalues + uvaluesafter + conflicts + where + ";";
                sv = new StrV("@s" + LLVM.n++, insert, insert.Length);
                local.llvm.strs.Add(sv);
                v2 = new Vari("i8*", "%ups");
                storefunc.comps.Add(new Gete("[" + (upsert.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                storefunc.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                v = new Vari("%StringType*", "%str" + LLVM.n++);
                storefunc.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", (upsert.Length * 1).ToString()), new Vari("i32", 1.ToString())));
                esql = new Vari("%ExecSqlType", "%esqlb");
                storefunc.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                coroptr = new Vari("%CoroFrameType**", "%coro");
                storefunc.comps.Add(new Alloca(coroptr));
                storefunc.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", n.ToString()), new Vari("ptr", "null"), new Vari("ptr", "null")));
                corov = new Vari("%CoroFrameType*", "%corovb");
                storefunc.comps.Add(new Load(corov, coroptr));
                storefunc.comps.Add(new Ret(corov));
                decstore = true;
            }

        }
        public bool decdelete;
        public void Delete(Local local)
        {
            if (!decdelete)
            {
                var tx = new Vari("i8*", "%tx");
                var obj = new Vari("%GCModel*", "%obj");
                var deletefunc = new Func(local.llvm, new Vari("ptr", "@DbStore" + this.identity), tx, obj);//async await tx data
                local.llvm.comps.Add(deletefunc);
                //if (id == 0) Insert()
                //else Upsert()
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                deletefunc.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));
                var idp = new Vari("i32*", "%idp");
                deletefunc.comps.Add(new Gete("%GCModel", idp, obj, new Vari("i32", "0"), new Vari("i32", "1")));
                var idv = new Vari("i32", "%idv");
                deletefunc.comps.Add(new Load(idv, idp));
                var compv = new Vari("i1", "%compv");
                deletefunc.comps.Add(new Eq(compv, idv, new Vari("i32", "0")));
                var lab1 = new Lab("none");
                var lab2 = new Lab("delete");
                deletefunc.comps.Add(new Br(compv, lab1, lab2));
                deletefunc.comps.Add(lab2);
                String na = TableName();
                var dele = "Delete from " + na + " where id = ?1;";
                int n = 0;
                var blk1 = draw.children[0] as Block;
                var blk2 = draw.children[1] as Block;
                var ps = new Vari("ptr", "%psa");
                deletefunc.comps.Add(new Call(ps, new Vari("ptr", "@malloc"), new Vari("i64", 24.ToString())));
                foreach (var kv in blk2.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (kv.Key == "id")
                        {
                            var pptr = new Vari("%SqlParamType*", "%p" + LLVM.n++);
                            deletefunc.comps.Add(new Gete("[" + n + " x %SqlParamType]", pptr, ps, new Vari("i32", "0"), new Vari("i32", n.ToString())));
                            var kptr = new Vari("i32*", "%k" + LLVM.n++);
                            deletefunc.comps.Add(new Gete("%SqlParamType", kptr, pptr, new Vari("i32", "0"), new Vari("i32", "0")));
                            deletefunc.comps.Add(new Store(kptr, new Vari("i32", "1")));
                            var vptr = new Vari("i32*", "%v" + LLVM.n++);
                            deletefunc.comps.Add(new Gete("%SqlParamType", vptr, pptr, new Vari("i32", "0"), new Vari("i32", "2")));
                            var ovp = new Vari("i32*", "%ov" + LLVM.n++);
                            deletefunc.comps.Add(new Gete(model, ovp, obj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                            var ov = new Vari("i32", "%ov" + LLVM.n++);
                            deletefunc.comps.Add(new Load(ov, ovp));
                            deletefunc.comps.Add(new Store(vptr, ov));
                        }
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                        }
                    }
                }
                var sv = new StrV("@s" + LLVM.n++, dele, dele.Length);
                local.llvm.strs.Add(sv);
                var v2 = new Vari("i8*", "%ups");
                deletefunc.comps.Add(new Gete("[" + (dele.Length + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
                deletefunc.comps.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
                var v = new Vari("%StringType*", "%str" + LLVM.n++);
                deletefunc.comps.Add(new Call(v, csvari, thgc4, v2, new Vari("i32", (dele.Length * 1).ToString()), new Vari("i32", 1.ToString())));
                var esql = new Vari("%ExecSqlType", "%esqlb");
                deletefunc.comps.Add(new Load(esql, new Vari("%ExecSqlType*", "@ExecSql")));
                var coroptr = new Vari("%CoroFrameType**", "%coro");
                deletefunc.comps.Add(new Alloca(coroptr));
                deletefunc.comps.Add(new Call(null, esql, coroptr, tx, v, ps, new Vari("i32", n.ToString()), new Vari("ptr", "null"), new Vari("ptr", "null")));
                var corov = new Vari("%CoroFrameType*", "%corovb");
                deletefunc.comps.Add(new Load(corov, coroptr));
                deletefunc.comps.Add(new Ret(corov));
                decdelete = true;
            }

        }
        bool decselect;
        public void Select(Local local)
        {
            if (!decselect)
            {
                var thgc = new Vari("%ThreadGCType*", "%thgc");
                var queue = new Vari("%CoroutineQueueType*", "%queue");
                var lk = new Vari("i8*", "%lock");
                var st = new Vari("i8*", "%st");
                var obj = new Vari(model + "*", "%obj");
                var selectfunc = new Func(local.llvm, new Vari("ptr", "@DbSelect" + this.identity), thgc, queue, lk, st, obj);//async await tx data
                //selectfunc.optnone = true;
                local.llvm.comps.Add(selectfunc);

                var lab1 = new Lab("start");
                selectfunc.comps.Add(new Br(null, lab1));
                selectfunc.comps.Add(lab1);
                var cof = new Vari("%CopyObjectType", "%cof");
                selectfunc.comps.Add(new Load(cof, new Vari("%CopyObjectType*", "@CloneObjectANT"), true));
                var newobj = new Vari(model + "*", "%nbj");
                selectfunc.comps.Add(new Call(newobj, cof, thgc, obj));
                var cond = new Vari("i1", "%cond");
                selectfunc.comps.Add(new Eq(cond, newobj, new Vari("ptr", "null")));
                var lab2 = new Lab("gc1");
                var lab3 = new Lab("ok1");
                selectfunc.comps.Add(new Br(cond, lab2, lab3));
                selectfunc.comps.Add(lab2);
                var gcf = new Vari("%GCANTType", "%gcf");
                selectfunc.comps.Add(new Load(gcf, new Vari("%GCANTType*", "@GCANT")));
                selectfunc.comps.Add(new Call(null, gcf, thgc, queue, lk));
                selectfunc.comps.Add(new Br(null, lab1));
                selectfunc.comps.Add(lab3);
                int n = 1;
                var blk1 = draw.children[0] as Block;
                foreach (var kv in blk1.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (variable.cls == local.Int)
                            {
                                var sqif = new Vari("%SqlIntType", "%sqif" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqif, new Vari("%SqlIntType*", "@SqlInt")));
                                var iv = new Vari("i32", "%iv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(iv, sqif, st, new Vari("i32", n.ToString())));
                                var ip = new Vari("i32*", "%ip" + LLVM.n++);
                                selectfunc.comps.Add(new Gete(model, ip, newobj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                selectfunc.comps.Add(new Store(ip, iv));
                            }
                            else if (variable.cls == local.Str)
                            {
                                var sqsf = new Vari("%SqlStringType", "%sqsf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqsf, new Vari("%SqlIntType*", "@SqlString")));
                                var sv = new Vari("i8*", "%sv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(sv, sqsf, st, new Vari("i32", n.ToString())));
                                var sqlf = new Vari("%SqlIntType", "%sqlf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqlf, new Vari("%SqlIntType*", "@SqlLength")));
                                var lv = new Vari("i32", "%lv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(lv, sqlf, st, new Vari("i32", n.ToString())));

                                var labn = new Lab("start" + LLVM.n++);
                                selectfunc.comps.Add(new Br(null, labn));
                                selectfunc.comps.Add(labn);
                                var csf = new Vari("%CreateStrintType", "%csf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(csf, new Vari("%CreateStringType*", "@CreateStringANT")));
                                var stv = new Vari("%StringType*", "%stv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(stv, csf, sv, lv, new Vari("i32", "1")));
                                var condn = new Vari("i1", "%cond" + LLVM.n++);
                                selectfunc.comps.Add(new Eq(condn, stv, new Vari("ptr", "null")));
                                var labn2 = new Lab("gc" + LLVM.n++);
                                var labn3 = new Lab("ok" + LLVM.n++);
                                selectfunc.comps.Add(new Br(condn, labn2, labn3));
                                selectfunc.comps.Add(labn2);
                                var gcfn = new Vari("%GCANTType", "%gcf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(gcfn, new Vari("%GCANTType*", "@GCANT")));
                                selectfunc.comps.Add(new Call(null, gcfn, thgc, queue, lk));
                                selectfunc.comps.Add(new Br(null, labn));
                                selectfunc.comps.Add(labn3);

                                var sp = new Vari("%StringType**", "%ip" + LLVM.n++);
                                selectfunc.comps.Add(new Gete(model, sp, newobj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                selectfunc.comps.Add(new Store(sp, stv));
                            }
                            n++;
                        }
                    }
                }
                var blk2 = draw.children[1] as Block;
                foreach (var kv in blk2.vmapA)
                {
                    if (kv.Value.type == ObjType.Variable)
                    {
                        var variable = kv.Value as Variable;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls.type == ObjType.ModelObj)
                        {
                            if (kv.Key == "id")
                            {
                                var sqif = new Vari("%SqlIntType", "%sqif" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqif, new Vari("%SqlIntType*", "@SqlInt")));
                                var iv = new Vari("i32", "%iv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(iv, sqif, st, new Vari("i32", "0")));
                                var ip = new Vari("i32*", "%ip" + LLVM.n++);
                                selectfunc.comps.Add(new Gete(model, ip, newobj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                selectfunc.comps.Add(new Store(ip, iv));
                                continue;
                            }
                            else if (variable.cls == local.Int)
                            {
                                var sqif = new Vari("%SqlIntType", "%sqif" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqif, new Vari("%SqlIntType*", "@SqlInt")));
                                var iv = new Vari("i32", "%iv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(iv, sqif, st, new Vari("i32", n.ToString())));
                                var ip = new Vari("i32*", "%ip" + LLVM.n++);
                                selectfunc.comps.Add(new Gete(model, ip, newobj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                selectfunc.comps.Add(new Store(ip, iv));
                            }
                            else if (variable.cls == local.Str)
                            {
                                var sqsf = new Vari("%SqlStringType", "%sqsf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqsf, new Vari("%SqlIntType*", "@SqlString")));
                                var sv = new Vari("i8*", "%sv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(sv, sqsf, st, new Vari("i32", n.ToString())));
                                var sqlf = new Vari("%SqlIntType", "%sqlf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(sqlf, new Vari("%SqlIntType*", "@SqlLength")));
                                var lv = new Vari("i32", "%lv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(lv, sqlf, st, new Vari("i32", n.ToString())));

                                var labn = new Lab("start" + LLVM.n++);
                                selectfunc.comps.Add(new Br(null, labn));
                                selectfunc.comps.Add(labn);
                                var csf = new Vari("%CreateStrintType", "%csf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(csf, new Vari("%CreateStringType*", "@CreateStringANT")));
                                var stv = new Vari("%StringType*", "%stv" + LLVM.n++);
                                selectfunc.comps.Add(new Call(stv, csf, sv, lv, new Vari("i32", "1")));
                                var condn = new Vari("i1", "%cond" + LLVM.n++);
                                selectfunc.comps.Add(new Eq(condn, stv, new Vari("ptr", "null")));
                                var labn2 = new Lab("gc" + LLVM.n++);
                                var labn3 = new Lab("ok" + LLVM.n++);
                                selectfunc.comps.Add(new Br(condn, labn2, labn3));
                                selectfunc.comps.Add(labn2);
                                var gcfn = new Vari("%GCANTType", "%gcf" + LLVM.n++);
                                selectfunc.comps.Add(new Load(gcfn, new Vari("%GCANTType*", "@GCANT")));
                                selectfunc.comps.Add(new Call(null, gcfn, thgc, queue, lk));
                                selectfunc.comps.Add(new Br(null, labn));
                                selectfunc.comps.Add(labn3);

                                var sp = new Vari("%StringType**", "%ip" + LLVM.n++);
                                selectfunc.comps.Add(new Gete(model, sp, newobj, new Vari("i32", "0"), new Vari("i32", variable.order.ToString())));
                                selectfunc.comps.Add(new Store(sp, stv));
                            }
                            n++;
                        }
                    }
                }
                selectfunc.comps.Add(new Ret(newobj));
                decselect = true;
            }

        }
    }
    partial class ModelVal
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.kouhos = new SortedList<string, Obj>();
                        foreach (var kv in vmap)
                        {
                            if (local.kouhos.ContainsKey(kv.Key)) continue;
                            local.kouhos.Add(kv.Key, kv.Value);
                        }
                        local.kouhos.Add("update", new Obj(ObjType.None));
                    }
                    var word = val2 as Word;
                    n++;
                    if (word.name == "update")
                    {
                        val2 = primary.children[n];
                        if (val2.type == ObjType.Bracket)
                        {
                            var val = val2.exeC(local).GetterC(local);
                            if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                            var block = val as Block;
                            n++;
                            if (block.rets.Count != 1) return Obj.Error(ObjType.Error, val2.letter, "updateの引数は1です");
                            var last = block.rets[0];
                            if (last.type == ObjType.Stock)
                            {
                                var stock = last as Stock;
                                return this;
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "updateの引数はDataStockを取ります");
                        }
                        else if (val2.type == ObjType.Stock)
                        {
                            return this;
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "updateの引数はDataStockを取ります");
                    }
                    else if (vmap.ContainsKey(word.name)) return vmap[word.name];
                    else return Obj.Error(ObjType.Error, val2.letter, "この変数には" + word.name + "は宣言されていません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の次は名前です");
            }
            return Obj.Error(ObjType.NG, val2.letter, "次は.ではありませんか");
        }
    }
    partial class StockType
    {

    }
    partial class Stock
    {
        public Obj SelectC(Object model, Local local, Function func)
        {
            var blk = func.draw.children[0] as Block;
            if (blk.vmapA.Count != 1) return Obj.Error(ObjType.Error, blk.letter, "Selectの引数は1つです");
            var ftype = new FuncType(local.Bool);
            ftype.draws.Add(model as Type);
            var ret = TypeCheck.CheckCVB(ftype, func, CheckType.Setter, local);
            if (ret.type == ObjType.Wait || ret.type == ObjType.Error || ret.type == ObjType.NG) return ret;
            return new Block(ObjType.Array) { rets = new List<Obj> { model as Obj } };
        }
        public Obj StoreC(Val val, Local local)
        {
            return new VoiVal();
        }
    }
    partial class Gene
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (letter == local.letter && local.kouhos == null)
            {
                local.calls.Last()();
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                val2 = primary.children[n];
                var gj = local.getC(word.name, local) as GeneObj;
                gj.letter = letter;
                if (val2.type == ObjType.CallBlock)
                {
                    gj.letter2 = (val2.children[1] as Block).letter2;
                    gj.call = val2 as CallBlock;
                    (gj.call.children[1] as Block).obj = new ObjBlock() { obj = gj, n = 1 };
                    foreach (var blk in local.blocks) gj.blocks.Add(blk);
                    n++;
                    val2 = primary.children[n];
                head:
                    if (val2.type == ObjType.Left)
                    {
                        n++;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.CallBlock)
                        {
                            gj.left = val2 as CallBlock;
                            n++;
                            goto head;
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "進化プログラミングの機能指定には{|}が必要です");
                    }
                    else if (val2.type == ObjType.Right)
                    {
                        n++;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.CallBlock)
                        {
                            gj.right = val2 as CallBlock;
                            n++;
                            goto head;
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "進化プログラミングの機能指定には{|}が必要です");
                    }
                    return gj;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "{|}を宣言してください。");
            }
            else if (val2.type == ObjType.CallBlock)
            {
                var gj = local.gene.vmap[""] as GeneObj;
                gj.call = val2 as CallBlock;
                (gj.call.children[1] as Block).obj = new ObjBlock() { obj = gj, n = 1 };
                foreach (var blk in local.blocks) gj.blocks.Add(blk);
                n++;
                val2 = primary.children[n];
            head:
                if (val2.type == ObjType.Left)
                {
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.CallBlock)
                    {
                        gj.left = val2 as CallBlock;
                        n++;
                        goto head;
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "進化プログラミングの機能指定には{|}が必要です");
                }
                else if (val2.type == ObjType.Right)
                {
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.CallBlock)
                    {
                        gj.right = val2 as CallBlock;
                        n++;
                        goto head;
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "進化プログラミングの機能指定には{|}が必要です");
                }
                return gj;

            }
            else if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    if (local.gene.vmap.ContainsKey(word.name))
                    {
                        return local.gene.vmap[word.name];
                    }
                    if (local.gene.vmap.ContainsKey(""))
                    {
                        var gj = local.gene.vmap[""] as GeneObj;
                        return gj.DotC(word.name);
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "無名Geneクラスは宣言されていません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前が必要です");
            }
            return Obj.Error(ObjType.NG, val2.letter, "geneの後に適切な単語が来ていません");
        }
    }
    partial class GeneChild
    {

        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            List<Obj> rets = new List<Obj>();
            if (val2.type == ObjType.Bracket)
            {
                if (val2.children.Count == 0)
                {
                    n++;
                    return Value.New(gj, local, letter);
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Childの引数は0です。");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneMutate
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                if (val2.children.Count == 0)
                {
                    n++;
                    return Value.New(gj, local, letter);
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Mutateの引数は0です。");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneNew
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                if (val2.children.Count == 0)
                {
                    n++;
                    return Value.New(gj, local, letter);
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Newの引数は0です。");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneCross
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                if (val2.children.Count == 2)
                {
                    var val = val2.exeC(local).GetterC(local);
                    if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                    var block = val as Block;
                    n++;
                    return Value.New(gj, local, letter);
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Crossの引数は2です。");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneVal
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    var vmap = ((cls as GeneObj).call.children[1] as Block).vmapA;
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.kouhos = new SortedList<string, Obj>();
                        foreach (var kv in vmap)
                        {
                            local.kouhos.Add(kv.Key, kv.Value);
                        }
                    }
                    if (vmap.ContainsKey(word.name)) return vmap[word.name];
                    else return Obj.Error(ObjType.Error, val2.letter, word.name + "は宣言されていません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前がきます");
            }
            return Obj.Error(ObjType.NG, val2.letter, "変数の後には.ではありませんか");
        }
    }
    partial class GeneStore
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                n++;
                if (val2.children.Count == 0)
                {
                    var value = Value.New(gj, local, letter);
                    if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                    local.db.StoreC(value as Val, local);
                    return value;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Storeの引数は0です。");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneSelect
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                if (val2.children.Count == 0)
                {
                    var value = Value.New(gj, local, letter);
                    if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                    n++;
                    return new Block(ObjType.Block) { rets = new List<Obj> { value } };
                }
                else if (val2.children.Count == 1)
                {
                    var value = Value.New(gj, local, letter);
                    if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                    var rets = new List<Obj> { value };
                    local.calls.Add(local.KouhoSet2);
                    var val = val2.exeC(local).GetterC(local);
                    local.calls.RemoveAt(local.calls.Count - 1);
                    if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                    var blk = val as Block;
                    if (blk.rets[0].type == ObjType.Function)
                    {
                        var func = blk.rets[0] as Function;
                        local.db.SelectC(gj, local, func);
                        n++;
                        return new Block(ObjType.Array) { rets = rets };
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[0], "Selectは関数を引数にとります");
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Selectの引数があっていません");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneSort
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                if (val2.children.Count == 0)
                {
                    var value = Value.New(gj, local, letter);
                    if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                    n++;
                    return new Block(ObjType.Block) { rets = new List<Obj> { value } };
                }
                else if (val2.children.Count == 1)
                {
                    local.calls.Add(local.KouhoSet2);
                    var val = val2.exeC(local).GetterC(local);
                    local.calls.RemoveAt(local.calls.Count - 1);
                    if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                    var blk = val as Block;
                    n++;
                    if (blk.rets[0].type == ObjType.Function)
                    {
                        var value = Value.New(gj, local, letter);
                        if (value.type == ObjType.Wait || value.type == ObjType.Error || value.type == ObjType.NG) return value;
                        var func = blk.rets[0] as Function;
                        return new Block(ObjType.Array) { rets = new List<Obj> { value } }.Sort(func, local);
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[0], "Sortは関数を引数にとります");
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Sortの引数があっていません");
            }
            return Obj.Error(ObjType.NG, val2.letter, "関数の後には()が必要です。");
        }
    }
    partial class GeneLabel
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
    }
    partial class GeneLabelValue
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.kouhos.Add("length", new Obj(ObjType.None));
                    }
                    var word = val2 as Word;
                    n++;
                    if (word.name == "length")
                    {
                        return new Number(id.labels.Count) { cls = local.Int };
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, "このGeneLabelには" + word.name + "は宣言されていません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後は名前でないといけません");
            }
            return Obj.Error(ObjType.NG, val2.letter, "変数の後には.ではありませんか");
        }
    }
    partial class NewBlock
    {
        public override Obj exeC(Local local)
        {
            rets.Add(new Number(0));
            return this;
        }
    }
    partial class CrossBlock
    {
        public override Obj exeC(Local local)
        {
            return Value.New(gj, local, letter);
        }
    }
    partial class MigrateBlock
    {
        public override Obj exeC(Local local)
        {
            rets.Add(new FloatVal(0.1f) { cls = local.Float });
            return this;
        }
    }
    partial class GeneObj
    {
        public Obj DotC(String name)
        {
            if (vmap.ContainsKey(name)) return vmap[name];
            return gv.vmap[name];
        }
        public override Obj exeC(Local local)
        {
            return this;
        }
    }
}
