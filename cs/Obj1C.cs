using Microsoft.EntityFrameworkCore.ChangeTracking.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace Cyclon
{
    partial class Obj
    {
        public static int cn = 32;
        public virtual String model
        {
            get { return _model; }
            set { _model = value; }
        }
        public virtual String call
        {
            get { return "?"; }
        }
        public virtual String drawcall
        {
            get { return "!"; }
        }
        public String _model = "";
        public Vari forvari;
        public int n;
        public int order;
        public Vari vari;
        public Vari bas;
        public Lab entry;
        public Lab end;
        public Dictionary<String, IfValue> ifv;
        public Dictionary<String, Func<String, Local, Obj, Obj>> opesC = new Dictionary<string, Func<string, Local, Obj, Obj>>();
        public List<Obj> awaits = new List<Obj>();
        public virtual Obj exeC(Local local)
        {
            throw new Exception();
        }
        public virtual Obj exepC(ref int n, Local local, Primary primary)
        {
            throw new Exception();
        }
        public virtual Obj GetterC(Local local)
        {
            return this;
        }
        public virtual Obj SelfC(Local local)
        {
            return this;
        }
        public virtual void Dec(Local local)
        {
            throw new Exception();
        }
        public virtual Obj opeC(String key, Local local, Obj val2)
        {
            if (opesC.ContainsKey(key)) return opesC[key](key, local, val2);
            return Obj.Error(ObjType.Error, letter, "オブジェクトに" + key + "が登録されていません");
        }
        public virtual Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            n++;
            return this;
        }
        public virtual Obj Finish(Local local)
        {
            return this;
        }
    }
    partial class Operator
    {
        public override Obj exeC(Local local)
        {
            if (name == "=" || name == ":" || name == "in")
            {
                var val2 = children[1].exeC(local).GetterC(local);
                var val1 = children[0].exeC(local).SelfC(local);
                return val1.opeC(name, local, val2);
            }
            else
            {
                var val1 = children[0].exeC(local).GetterC(local);
                if (val1.type == ObjType.Wait || val1.type == ObjType.Error || val1.type == ObjType.NG) return val1;
                var val2 = children[1].exeC(local).GetterC(local);
                if (val2.type == ObjType.Wait || val2.type == ObjType.Error || val2.type == ObjType.NG) return val2;
                return val1.opeC(name, local, val2);
            }
            throw new Exception();
        }
    }
    partial class Primary
    {
        public static Obj NextC(Primary primary, Local local, ref int i, ref Obj val1)
        {
            switch (primary.children[i].type)
            {
                case ObjType.Comment2:
                    i++;
                    break;
                case ObjType.Htm:
                case ObjType.TagBlock:
                    primary.children[i].exepC(ref i, local, primary);
                    i++;
                    val1 = null;
                    break;
                case ObjType.Gene:
                case ObjType.Model:
                case ObjType.Class:
                case ObjType.Word:
                case ObjType.Number:
                case ObjType.FloatVal:
                case ObjType.BoolVal:
                case ObjType.StrObj:
                case ObjType.Bracket:
                case ObjType.Print:
                case ObjType.Return:
                case ObjType.Goto:
                case ObjType.Continue:
                case ObjType.Var:
                case ObjType.If:
                case ObjType.While:
                case ObjType.For:
                case ObjType.Switch:
                case ObjType.GeneLabel:
                case ObjType.Comment:
                case ObjType.Client:
                case ObjType.Server:
                case ObjType.Signal:
                case ObjType.ServerClient:
                case ObjType.Dolor:
                case ObjType.Mountain:
                    if (val1 == null)
                    {
                        val1 = primary.children[i].exepC(ref i, local, primary);
                        i++;
                    }
                    else
                    {
                        val1 = val1.GetterC(local);
                        if (val1.type == ObjType.Wait || val1.type == ObjType.Error || val1.type == ObjType.NG) return val1;
                        val1 = val1.PrimaryC(ref i, local, primary, primary.children[i].Clone());
                    }
                    break;
                case ObjType.Dot:
                case ObjType.Left:
                case ObjType.Right:
                    val1 = val1.GetterC(local);
                    if (val1.type == ObjType.Wait || val1.type == ObjType.Error || val1.type == ObjType.NG) return val1;
                    val1 = val1.PrimaryC(ref i, local, primary, primary.children[i]);
                    break;
                case ObjType.CallBlock:
                case ObjType.Block:
                    if (val1 == null)
                    {
                        val1 = primary.children[i].exepC(ref i, local, primary);
                        i++;
                    }
                    else
                    {
                        val1 = val1.GetterC(local);
                        if (val1.type == ObjType.Wait || val1.type == ObjType.Error || val1.type == ObjType.NG) return val1;
                        val1 = val1.PrimaryC(ref i, local, primary, primary.children[i]);
                    }
                    break;
                default:
                    throw new Exception();
            }
            return val1;
        }
        public override Obj exeC(Local local)
        {
            if (local.comments.Count > 0) local.comments.Last().Start();
            Obj val1 = null;
            for (var i = 0; i < children.Count - 1;)
            {
                NextC(this, local, ref i, ref val1);
                if (val1 != null && (val1.type == ObjType.Wait || val1.type == ObjType.Error || val1.type == ObjType.NG)) return val1;
            }
            if (val1 == null) val1 = children.Last();
            SingleOp op = null;
            for (int i = singleops.Count - 1; i >= 0; i--)
            {
                var str = singleops[i].name;
                val1 = val1.GetterC(local);
                val1 = val1.opeC(str, local, null); break;
            }
            return val1;
        }
    }
    partial class Block
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (type == ObjType.Block)
            {
                var m = 0;
                if (children[m].type == ObjType.Word)
                {
                    if ((children[m] as Word).name == "T")
                    {
                        m++;
                        var val2 = children[m];
                        Type type = new VariClass(0);
                    head:
                        if (val2.type == ObjType.Block)
                        {
                            var blk = val2 as Block;
                            if (blk.children.Count == 0)
                            {
                                type = new ArrType(type);
                                m++;
                                val2 = children[m];
                                goto head;
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "配列型は引数が0です");
                        }
                        n = 1;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.Word)
                        {
                            var word = val2 as Word;
                            n++;
                            val2 = primary.children[n];
                            if (val2.type == ObjType.Block)
                            {
                                var func = local.declareC(word.name, local) as GenericFunction;
                                var blk = val2.exe(local) as Block;
                                for (var i = 0; i < blk.rets.Count; i++)
                                {
                                    if (blk.rets[i].type == ObjType.Word)
                                    {
                                        var word2 = blk.rets[i] as Word;
                                        func.vmap[word2.name] = new VariClass(i);
                                    }
                                    else return Obj.Error(ObjType.Error, val2.letter, "ジェネリックスの引数は名前を指定してください");
                                }
                                n++;
                                val2 = primary.children[n];
                                if (val2.type == ObjType.CallBlock)
                                {
                                    func.draw = val2 as CallBlock;
                                    foreach (var b in local.blocks) func.blocks.Add(b);
                                    return func;
                                }
                                return Obj.Error(ObjType.Error, val2.letter, "ジェネリックス関数の{|}が宣言されていません");
                            }
                            else if (val2.type == ObjType.CallBlock)
                            {
                                var func = local.declareC(word.name, local) as Function ;
                                func.draw = val2 as CallBlock;
                                foreach (var b in local.blocks) func.blocks.Add(b);
                                
                                return func;
                            }
                            return Obj.Error(ObjType.Error, val2.letter, "ジェネリックス関数の{|}が宣言されていません");
                        }
                    }
                }
            }
            if (type == ObjType.Block)
            {
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                var vcl = new Vari("%CreateHashType", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(vcl, new Vari("%CreateHashType*", "@CreateHash")));
                var voj = new Vari("%HashType*", "%v" + LLVM.n++);
                if (cls.type == ObjType.Var) cls = (cls as Var).cls;
                int size = 8;
                bool objected = true;
                if (cls == local.Int)
                {
                    size = 4;
                    objected = false;
                }
                else if (cls == local.Bool)
                {
                    size = 1;
                    objected = false;
                }
                local.llvm.func.comps.Add(new Call(voj, vcl, thgc4, new Vari("i1", objected.ToString().ToLower())));
                var blk = exeRangeC(0, children.Count, local, false) as Block;
                var varih = new Vari("%AddHashType", "%v" + LLVM.n++);
                local.llvm.func.comps.Add(new Load(varih, new Vari("%AddHashType*", "@AddHash")));
                for (var i = 0; i < blk.rets.Count; i++)
                {
                    if (blk.labelmapn.ContainsKey(i))
                    {
                        if (objected)
                        {
                            local.llvm.func.comps.Add(new Call(null, varih, thgc4, voj, new Vari("%StringType*", blk.labelmapn[i].name), rets[i].vari));
                        }
                        else
                        {
                            var v0 = new Vari("i8*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new IntToPtr(v0, rets[i].vari));
                            local.llvm.func.comps.Add(new Call(null, varih, thgc4, voj, new Vari("%StringType*", blk.labelmapn[i].name), v0));
                        }
                    }
                    else
                    {
                        if (objected)
                        {
                            local.llvm.func.comps.Add(new Call(null, varih, voj, new Vari("%StringType*", "null"), rets[i].vari));
                        }
                        else
                        {
                            var v0 = new Vari("i8*", "%v" + LLVM.n++);
                            local.llvm.func.comps.Add(new IntToPtr(v0, rets[i].vari));
                            local.llvm.func.comps.Add(new Call(null, varih, thgc4, voj, new Vari("%StringType*", "null"), v0));
                        }
                    }
                }
                return new Value(new ArrType(cls), voj);
            }
            return exeRangeC(0, children.Count, local, true);
        }
        public override Obj exeC(Local local)
        {
            if (type == ObjType.Array) return this;
            return exeRangeC(0, children.Count, local, true);
        }
        public Obj exeRangeC(int start, int end, Local local, bool id)
        {
            rets = new List<Obj>();
            for (var i = start; i < end; i++)
            {
                Element l;
                for (l = children[i].letter.parent; l != null; l = l.parent)
                {
                    if (l.type == LetterType.Line) break;
                }
                var li = l as Line;
                local.llvm.func.comps.Add(new LocationMarker(li.line, i, local.llvm.func.debugId));
                var val = children[i].exeC(local);
                rets.Add(val);
            }
            if (start == end)
            {

                int line = 1;
                if (this.letter != null)
                {
                    for (Element l = this.letter.parent; l != null; l = l.parent)
                    {
                        if (l.type == LetterType.Line) line = (l as Line).line;
                    }
                }
                local.llvm.func.comps.Add(new LocationMarker(line, 0, local.llvm.func.debugId));
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (type == ObjType.Bracket)
            {
                if (val2.type == ObjType.Block)
                {
                    if (rets.Count == 1)
                    {
                        if (val2.children.Count == 0)
                        {
                            var cls = rets[0] as Type;
                            if (cls != null)
                            {
                                var typearray = new ArrType(cls);
                                return typearray;
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "配列方の宣言は型を元にしてください");
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "配列型の宣言の[]は引数が0です");
                    }
                    else return Obj.Error(ObjType.Error, letter, "配列方の宣言は1つの型を配列にしてください");
                }
                else if (val2.type == ObjType.Bracket)
                {
                    var block = val2.exe(local) as Block;
                    n++;
                    val2 = primary.children[n];
                    for (var i = 0; i < block.rets.Count; i++)
                    {
                        val2 = block.rets[i];
                        if (val2.type == ObjType.Word)
                        {
                            var cls = rets[i % rets.Count] as Type;
                            if (cls != null)
                            {
                                var variable = local.declareC((val2 as Word).name, local);
                                block.rets[i] = variable;
                            }
                            else return Obj.Error(ObjType.Error, letters[i % rets.Count], "複数型の変数宣言は型を指定してください");
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "複数の変数の宣言には名前を指定してください");
                    }
                    return block;
                }
                else if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    val2 = primary.children[n];
                    var variable = local.declareC(word.name, local);
                    return variable;
                }
                else if (val2.type == ObjType.Left)
                {
                    n++;
                    val2 = primary.children[n];
                    if (rets.Count == 1)
                    {
                        return rets[0].PrimaryC(ref n, local, primary, val2);
                    }
                    else return Obj.Error(ObjType.Error, letter, "この()は1つしかとれない");

                }
                else if (val2.type == ObjType.Dot)
                {
                    if (rets.Count == 1)
                    {
                        return rets[0].PrimaryC(ref n, local, primary, val2);
                    }
                    else return Obj.Error(ObjType.Error, letter, "この()は1つしかとれない");
                }
                else return Obj.Error(ObjType.NG, val2.letter, "()の後が必要です");
            }
            else if (type == ObjType.Block || type == ObjType.Array)
            {
                if (val2.type == ObjType.Block)
                {
                    throw new Exception("");
                }
                else if (val2.type == ObjType.Dot)
                {
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.Word)
                    {
                        if (val2.letter == local.letter && local.kouhos == null)
                        {
                            local.kouhos = new SortedList<string, Obj>();
                            local.kouhos.Add("length", new Obj(ObjType.None));
                            local.kouhos.Add("Sort", new Obj(ObjType.None));
                            local.kouhos.Add("Range", new Obj(ObjType.None));
                        }
                        var word = val2 as Word;
                        n++;
                        if (word.name == "length")
                        {
                            return new Number(rets.Count) { cls = local.Int };
                        }
                        else if (word.name == "Sort")
                        {
                            val2 = primary.children[n];
                            if (val2.type == ObjType.Bracket)
                            {
                                n++;
                                var val = val2.exeC(local).GetterC(local);
                                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                                var blk1 = val as Block;
                                if (blk1.rets[0].type == ObjType.Function)
                                {
                                    return SortC(blk1.rets[0] as Function, local);
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Sortの引数は関数しかだめです");
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "Sortの()がありません");
                        }
                        else if (word.name == "Range")
                        {
                            val2 = primary.children[n];
                            if (val2.type == ObjType.Bracket)
                            {
                                var val = val2.exeC(local).GetterC(local);
                                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                                var blk = val as Block;
                                if (blk.rets.Count == 0) { }
                                else if (blk.rets.Count == 1)
                                {
                                    if (blk.rets[0].type != ObjType.Number) return Obj.Error(ObjType.Error, blk.letters[0], "Rangeの引数1は整数です");
                                }
                                else if (blk.rets.Count == 2)
                                {
                                    if (blk.rets[0].type != ObjType.Number) return Obj.Error(ObjType.Error, blk.letters[0], "Rangeの引数1は整数です");
                                    if (blk.rets[1].type != ObjType.Number) return Obj.Error(ObjType.Error, blk.letters[0], "Rangeの引数2は整数です");
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Rangeの引数は0,1,2です");
                                n++;
                                return this;
                            }
                            else return Obj.Error(ObjType.Error, val2.letter, "Rangeの()がありません");
                        }
                        else return Obj.Error(ObjType.NG, val2.letter, "配列には" + word.name + "は宣言されていません");
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前が必要です");
                }
                else return Obj.Error(ObjType.NG, val2.letter, "[]の後が必要です");
            }
            return Obj.Error(ObjType.Error, val2.letter, "不正なエラー");
        }
        public override Obj SelfC(Local local)
        {
            for (var i = 0; i < rets.Count; i++)
            {
                rets[i] = rets[i].SelfC(local);
                if (rets[i].type == ObjType.Wait || rets[i].type == ObjType.Error || rets[i].type == ObjType.NG) return rets[i];
            }
            return this;
        }
        public override Obj GetterC(Local local)
        {
            for (var i = 0; i < rets.Count; i++)
            {
                rets[i] = rets[i].GetterC(local);
                if (rets[i].type == ObjType.Wait || rets[i].type == ObjType.Error || rets[i].type == ObjType.NG) return rets[i];
            }
            return this;
        }
        public Obj EqualC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Block)
            {
                var blk = val2 as Block;
                for (var i = 0; i < rets.Count; i++)
                {
                    (rets[i] as Variable).value = blk.rets[i % blk.rets.Count];
                }
            }
            else
            {
                for (var i = 0; i < rets.Count; i++)
                {
                    (rets[i] as Variable).ope(op, local, val2);
                }
            }
            return this;
        }
        public Obj InC(String op, Local local, Obj val2)
        {
            if (val2.type == ObjType.Block)
            {
                for (var i = 0; i < rets.Count; i++)
                {
                    var it = new Iterator(i);
                    it.value = val2 as Block;
                    (rets[i] as Variable).value = it;
                }
                return this;
            }
            else return Obj.Error(ObjType.Error, val2.letter, "イテレーターには配列を代入してください");
        }
        public Obj OpeC(String op, Local local, Obj val2)
        {
            if (type == ObjType.Bracket)
            {
                if (rets.Count == 1)
                {
                    return rets[0].ope(op, local, val2);
                }
                else return Obj.Error(ObjType.Error, letter, "()は1つだけです");
            }
            else return Obj.Error(ObjType.Error, letter, "演算子は()だけです");
        }
        public Obj SortC(Function func, Local local)
        {
            var ftype = new FuncType(local.Bool);
            ftype.draws.Add((rets[0] as Val).cls);
            ftype.draws.Add((rets[0] as Val).cls);
            var ret = TypeCheck.CheckCVB(ftype, func, CheckType.Setter, local);
            if (ret.type == ObjType.Wait || ret.type == ObjType.Error || ret.type == ObjType.NG) return ret;
            return this;
        }
    }
    partial class CallBlock
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj exeC(Local local)
        {
            local.llvm = new LLVM(local);
            local.migrate = new Migrate();
            local.migrate.Start();

            var func = new Func(local.llvm, new Vari("void", "@main"), new Vari("i32", "%cn"));
            local.llvm.comps.Add(func);
            local.llvm.main = func;

            local.llvm.main.comps.Add(new Store(new Vari("i32*", "@cnp"), new Vari("i32", "%cn")));

            var typedec = new TypeDec("%mainType");
            local.llvm.types.Add(typedec);

            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var funccheck = new Func(local.llvm, new Vari("void", "@mainCheck"), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            typedec.comps.Add(new TypeVal("i8*", "blk"));
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete("%mainType", vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari2);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            local.llvm.main.comps.Add(new Load(thgc3, new Vari("%ThreadGCType**", "@thgcp")));
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            local.llvm.main.comps.Add(new Load(ac_val, new Vari("%GC_AddClassType*", "@GC_AddClass")));
            var countv = new Vari("i32", "");
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var name = "m";
            var strv = new StrV("@" + name, name, name.Length * 1);
            local.llvm.strs.Add(strv);
            var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", "@mainCheck"), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            local.llvm.func.dbinits.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

            var rngv = new Vari("%RootNodeType**", "%rngv");
            local.llvm.func.dbinits.Add(new Gete("%ThreadGCType", rngv, thgc4, new Vari("i32", "0"), new Vari("i32", "1")));
            var rn4 = new Vari("%RootNodeType*", "%rn");
            local.llvm.func.dbinits.Add(new Load(rn4, rngv));

            var go_val = new Vari("%GC_mallocType", "%gcobject");
            var go_load = new Load(go_val, new Vari("%GC_mallocType*", "@GC_malloc"));
            local.llvm.func.comps.Add(go_load);
            var go_v = new Vari("%GCObjectPtr", "%obj");
            var tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            local.llvm.func.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            local.llvm.func.comps.Add(new Add(tmv, tmi, tmp));
            var go_call = new Call(go_v, go_val, thgc4, tmv);
            local.llvm.func.comps.Add(go_call);
            var go_c = new Vari("%mainType*", "%v" + LLVM.n++);
            /*Element l;
            for (l = this.letter.parent; l != null; l = l.parent)
            {
                if (l.type == LetterType.Line) break;
            }
            var li = l as Line;*/
            local.llvm.func.comps.Add(new LocationMarker(1, 0, local.llvm.func.debugId));
            var go_cast = new Bitcast(go_c, go_v);

            int structTypeId = local.llvm.debugInfo.GetOrCreateType("%mainType");
            int ptrTypeId = local.llvm.debugInfo.GetPointerType(structTypeId);
            int varId = local.llvm.debugInfo.metaId++;

            var debugVar = new DebugVariable(
                "mainObj",  // LLDBで表示される名前
                ptrTypeId,
                local.llvm.func.debugId,
                1,
                varId
            );
            local.llvm.func.debugVariables.Add(debugVar);

            // bitcastにデバッグ情報を設定
            go_cast.debugVarId = varId;

            local.llvm.func.comps.Add(go_cast);

            var objptr = new Vari("%mainType**", "%objptr");
            var alloca = new Alloca(objptr);
            local.llvm.func.comps.Add(alloca);
            var objstore = new Store(objptr, go_c);
            local.llvm.func.comps.Add(objstore);

            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            local.llvm.func.comps.Add(srload);
            var srcall = new Call(null, srv, rn4, objptr);
            local.llvm.func.comps.Add(srcall);

            int gn = 0;
            int order = 1;
            int count = 8;
            var blk = children[1] as Block;
            blk.obj.obj.model = "%mainType";
            blk.obj.obj.ifv = new Dictionary<string, IfValue>();
            local.blocks = new List<Block>();
            local.blocks.Add(blk);
            var decs = new List<Obj>();
            var vals = new List<StructField>();
            foreach (var kv in blk.vmapA)
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
                        typedec.comps.Add(new TypeVal("i2", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete("%mainType", vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
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
                    gete = new Gete("%mainType", vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
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
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), rn4, go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    local.llvm.func.comps.Add(fu_call);
                    var va2 = new Vari("%mainType*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(va2, objptr));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete("%mainType", va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
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
                    gete = new Gete("%mainType", vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
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
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), rn4, go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    local.llvm.func.comps.Add(fu_call);
                    var va2 = new Vari("%mainType*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(va2, objptr));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete("%mainType", va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    local.llvm.func.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    local.llvm.func.comps.Add(fu_store);
                }
                else if (v.type == ObjType.ModelObj)
                {
                    var f = v as ModelObj;
                    if (f.initial) continue;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete("%mainType", vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
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
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), rn4, go_c, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    local.llvm.func.comps.Add(fu_call);
                    var va2 = new Vari("%mainType*", "%v" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(va2, objptr));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete("%mainType", va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    local.llvm.func.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    local.llvm.func.comps.Add(fu_store);
                }
            }
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            countv.name = count.ToString();
            funccheck.comps.Add(new Ret(new Vari("void", null)));

            blk.obj.obj.bas = objptr;
            local.labs.Add(new Lab("entry"));
            var obj = blk.exeC(local) as Block;
            local.labs.RemoveAt(local.labs.Count - 1);

            var rnpv = new Vari("i32*", "%ptr");
            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
            var rnpv12 = new Vari("i32", "%v");
            var rnp12load = new Load(rnpv12, rnpv);
            local.llvm.func.comps.Add(rnp12load);
            var rnpv2 = new Vari("i32", "%dec");
            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            local.llvm.func.comps.Add(rnpsub);
            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
            if (obj.rets.Count == 0) local.llvm.func.comps.Add(new Ret(new Vari("i32", "0")));
            else
            {
                var valobj = obj.rets.Last().GetterC(local);
                if (valobj.type == ObjType.Value)
                {
                    var value = valobj as Value;
                    if (value.cls == local.Int || value.cls == local.Short || value.cls == local.Bool)
                    {
                        var va = Bitcast.Cast(local, local.Int, value.cls, valobj.vari);
                        local.llvm.func.comps.Add(new Ret(va));
                    }
                    else return Obj.Error(ObjType.Error, valobj.letter, "最後の値は整数型でなければなりません");
                }
                else return Obj.Error(ObjType.Error, valobj.letter, "最後の値はValue型でなければなりません");
            }
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.blockslist.RemoveAt(local.blockslist.Count - 1);
            func.comps.Add(new Ret(new Vari("void", null)));
            var csvari = new Vari("%CreateStringType", "%v" + LLVM.n++);
            local.llvm.func.dbinits.Add(new Load(csvari, new Vari("%CreateStringType*", "@CreateString")));
            int len1 = local.migrate.migrations.Last().fulltext.Length;
            var sd1 = new StrV("@dbtex", local.migrate.migrations.Last().fulltext.Replace("\n", "\\0A"), len1);
            local.llvm.strs.Add(sd1);
            var vd1 = new Vari("i8*", "%v" + LLVM.n++);
            local.llvm.func.dbinits.Add(new Gete("[" + (len1 + 1) + " x i8]", vd1, sd1, new Vari("i32", "0"), new Vari("i32", "0")));
            var vdd1 = new Vari("%StringType*", "%v" + LLVM.n++);
            local.llvm.func.dbinits.Add(new Call(vdd1, csvari, thgc4, vd1, new Vari("i32", len1.ToString()), new Vari("i32", "1")));
            int len2 = "db".Length;
            var sd2 = new StrV("@db", "db", len2);
            local.llvm.strs.Add(sd2);
            var vd2 = new Vari("i8*", "%v" + LLVM.n++);
            local.llvm.func.dbinits.Add(new Gete("[" + (len2 + 1) + " x i8]", vd2, sd2, new Vari("i32", "0"), new Vari("i32", "0")));
            var vdd2 = new Vari("%StringType*", "%v" + LLVM.n++);
            local.llvm.func.dbinits.Add(new Call(vdd2, csvari, thgc4, vd2, new Vari("i32", len2.ToString()), new Vari("i32", "1")));
            var dbinit = new Vari("%DbInitType", "%v" + LLVM.n++);
            local.llvm.func.dbinits.Add(new Load(dbinit, new Vari("%DbInitType*", "@DbInit")));
            var sqlp = new Vari("ptr", "%sqlp");
            local.llvm.func.dbinits.Add(new Call(sqlp, dbinit, thgc4, vdd2, new Vari("i32", "0"), new Vari("i32", "0"), new Vari("i32", "0"), vdd1));
            local.llvm.func.dbinits.Add(new Store(new Vari("ptr", "@sqlp"), sqlp));
            return this;
        }

    }
    partial class Local
    {
        public LLVM llvm;
        public List<Component> comps;
        public List<Lab> labs = new List<Lab>();
        public Migrate migrate;
        public Vari go_c, i8p, countv, objptr3, co_val, blok;
        public TypeDec typedec;
        public int count, odr;
        public Func funcdec, funccheck;
        public Obj declareC(String name, Local local)
        {
            return getC(name, local);
        }
        public Obj getC(String name, Local local)
        {
            int count = 0;
            var outside = false;
            Vari vari = null;
            Block oldblk = null;
            comps = new List<Component>();
            for (var i = 0; i < blocks.Count; i++)
            {
                Component loaded = null;
                var blk = blocks[blocks.Count - i - 1];
                if (blk.obj.obj.type == ObjType.Comment)
                {
                    if (local.llvm.func.async)
                    {
                        var obj = new Vari("%CoroFrameType*", "%frame");
                        var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                        comps.Add(new Gete("%CoroFrameType", ogv, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                        vari = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                        comps.Add(new Load(vari, ogv));
                        outside = true;
                    }
                    else
                    {
                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        var load = new Load(vari, blk.obj.obj.bas);
                        comps.Add(load);
                        outside = true;
                    }
                }
                else if (i == 0 || blk.obj.n == 1 || blk.obj.n == -1)
                {
                    if (blk.obj.obj.type == ObjType.While || blk.obj.obj.type == ObjType.For || blk.obj.obj.type == ObjType.Model)
                    {
                        if (outside)
                        {
                            var newvari = new Vari("i8**", "%v" + LLVM.n++);
                            var gete = new Gete(oldblk.obj.obj.model, newvari, vari, new Vari("i32", "0"), new Vari("i32", "0"));
                            comps.Add(gete);
                            var newvari2 = new Vari("i8*", "%v" + LLVM.n++);
                            comps.Add(new Load(newvari2, newvari));
                            vari = newvari2;
                        }
                        else
                        {
                            if (local.llvm.func.async)
                            {
                                var obj = new Vari("%CoroFrameType*", "%frame");
                                var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                                comps.Add(new Gete("%CoroFrameType", ogv, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                vari = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                                comps.Add(new Load(vari, ogv));
                                outside = true;
                            }
                            else
                            {
                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                var load = new Load(vari, blk.obj.obj.bas);
                                loaded = load;
                            }
                        }
                    }
                    else if (blk.obj.obj.type == ObjType.IfBlock)
                    {
                        if (outside)
                        {
                            var newvari = new Vari("i8**", "%v" + LLVM.n++);
                            var gete = new Gete(oldblk.obj.obj.model, newvari, vari, new Vari("i32", "0"), new Vari("i32", "0"));
                            comps.Add(gete);
                            var newvari2 = new Vari("i8*", "%v" + LLVM.n++);
                            comps.Add(new Load(newvari2, newvari));
                            vari = newvari2;
                        }
                        else
                        {
                            if (local.llvm.func.async)
                            {
                                var obj = new Vari("%CoroFrameType*", "%frame");
                                var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                                comps.Add(new Gete("%CoroFrameType", ogv, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                vari = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                                comps.Add(new Load(vari, ogv));
                                outside = true;
                            }
                            else
                            {
                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                var load = new Load(vari, (blk.obj.obj as IfBlock).iflabel.bas);
                                loaded = load;
                            }
                        }
                    }
                    else if (blk.obj.obj.type == ObjType.None || blk.obj.obj.type == ObjType.Function || blk.obj.obj.type == ObjType.ClassObj || blk.obj.obj.type == ObjType.ModelObj || blk.obj.obj.type == ObjType.ServerFunction || blk.obj.obj.type == ObjType.SignalFunction)
                    {
                        if (outside)
                        {
                            var newvari = new Vari("i8**", "%v" + LLVM.n++);
                            var gete = new Gete(oldblk.obj.obj.model, newvari, vari, new Vari("i32", "0"), new Vari("i32", "0"));
                            comps.Add(gete);
                            var newvari2 = new Vari("i8*", "%v" + LLVM.n++);
                            comps.Add(new Load(newvari2, newvari));
                            vari = newvari2;
                        }
                        else
                        {
                            if (local.llvm.func.async)
                            {
                                var obj = new Vari("%CoroFrameType*", "%frame");
                                var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                                comps.Add(new Gete("%CoroFrameType", ogv, obj, new Vari("i32", "0"), new Vari("i32", "5")));
                                vari = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
                                comps.Add(new Load(vari, ogv));
                                outside = true;
                            }
                            else
                            {
                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                var load = new Load(vari, blk.obj.obj.bas);
                                comps.Add(load);
                            }
                        }
                        outside = true;
                    }
                }
                else
                {
                    if (blk.obj.obj.type == ObjType.While || blk.obj.obj.type == ObjType.For)
                    {
                        if (!outside)
                        {
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            var load = new Load(vari, blk.obj.obj.bas);
                            loaded = load;
                        }
                    }
                    else if (blk.obj.obj.type == ObjType.IfBlock)
                    {
                        if (!outside)
                        {
                            vari = new Vari("i8*", "%v" + LLVM.n++);
                            var load = new Load(vari, (blk.obj.obj as IfBlock).iflabel.bas);
                            loaded = load;
                        }
                    }
                }
                if (blk.vmapA.ContainsKey(name))
                {
                    if (blk.obj.obj.type == ObjType.TagBlock)
                    {
                        for(int j = i + 1; j < blocks.Count; j++)
                        {
                            var b = blocks[blocks.Count - j - 1];
                            if (blk.obj.obj.type == ObjType.TagBlock)
                            {
                                continue;
                            }
                            else if (b.obj.obj.type == ObjType.IfBlock)
                            {
                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                var load = new Load(vari, (blk.obj.obj as IfBlock).iflabel.bas);
                                loaded = load;
                                break;
                            }
                            else
                            {
                                vari = new Vari("i8*", "%v" + LLVM.n++);
                                var load = new Load(vari, blk.obj.obj.bas);
                                loaded = load;
                            }
                        }
                    }
                    var obj = blk.vmapA[name];
                    if (loaded != null) comps.Add(loaded);
                    var newvari = new Vari("i8*", "%v" + LLVM.n++);
                    var gete = new Gete(blk.obj.obj.model, newvari, vari, new Vari("i32", "0"), new Vari("i32", obj.order.ToString()));
                    comps.Add(gete);
                    local.llvm.func.vari = newvari;
                    if (obj.type == ObjType.Variable)
                    {
                        return obj.SelfC(local);
                    }
                    return obj;
                }
                oldblk = blk;
                count++;
            }
            if (local.sigmapA.ContainsKey(name)) return local.sigmapA[name];
            return Obj.Error(ObjType.Error, null, name + "という変数が見つかりません");
        }
        public Obj getC2(CallBlock call)
        {
            return vmapA2[call.n];
        }
    }
    partial class CommentLet
    {
        public async void SingleC(SingleOp op, Local local)
        {
            if (op == null)
            {
                foreach (var elem in instances) elems[elems.Count - 1].add(elem);
            }
            else if (instances.Count > 0)
            {
                var line = new Line(local.panel);
                if (op.name == ".")
                {
                    line.add(new Letter(local.panel) { text = "･", name = "･", type = LetterType.Htm });
                    nums[nums.Count - 1] = 0;
                }
                else if (op.name == "*")
                {
                    nums[nums.Count - 1]++;
                    line.add(new Letter(local.panel) { text = nums.Last() + ".", name = nums.Last() + ".", type = LetterType.Htm });
                }
                else if (op.name == ">>")
                {
                    op.letter.text = "--";
                    op.letter.type = LetterType.CommentSingle;
                    op.letter.parent.update = true;
                    op.letter.parent.recompile = true;
                    var text = "";
                    foreach (var elem in instances)
                    {
                        text += elem.Text;
                    }
                    await local.panel.form.OPI(op.letter.parent as Line, op.letter, text, local);
                    instanceslist.RemoveAt(instanceslist.Count - 1);
                    nums[nums.Count - 1] = 0;
                    return;
                }
                else nums[nums.Count - 1] = 0;
                foreach (var elem in instances) line.add(elem);
                elems[elems.Count - 1].add(line);
            }
            instanceslist.RemoveAt(instanceslist.Count - 1);
        }
    }
}
