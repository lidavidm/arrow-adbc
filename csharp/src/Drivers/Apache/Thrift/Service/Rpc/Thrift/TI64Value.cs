/**
 * <auto-generated>
 * Autogenerated by Thrift Compiler (0.17.0)
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 * </auto-generated>
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Thrift;
using Thrift.Collections;
using Thrift.Protocol;
using Thrift.Protocol.Entities;
using Thrift.Protocol.Utilities;
using Thrift.Transport;
using Thrift.Transport.Client;
using Thrift.Transport.Server;
using Thrift.Processor;


#pragma warning disable IDE0079  // remove unnecessary pragmas
#pragma warning disable IDE0017  // object init can be simplified
#pragma warning disable IDE0028  // collection init can be simplified
#pragma warning disable IDE1006  // parts of the code use IDL spelling
#pragma warning disable CA1822   // empty DeepCopy() methods still non-static
#pragma warning disable IDE0083  // pattern matching "that is not SomeType" requires net5.0 but we still support earlier versions

namespace Apache.Hive.Service.Rpc.Thrift
{

  public partial class TI64Value : TBase
  {
    private long _value;

    public long Value
    {
      get
      {
        return _value;
      }
      set
      {
        __isset.@value = true;
        this._value = value;
      }
    }


    public Isset __isset;
    public struct Isset
    {
      public bool @value;
    }

    public TI64Value()
    {
    }

    public TI64Value DeepCopy()
    {
      var tmp103 = new TI64Value();
      if (__isset.@value)
      {
        tmp103.Value = this.Value;
      }
      tmp103.__isset.@value = this.__isset.@value;
      return tmp103;
    }

    public async global::System.Threading.Tasks.Task ReadAsync(TProtocol iprot, CancellationToken cancellationToken)
    {
      iprot.IncrementRecursionDepth();
      try
      {
        TField field;
        await iprot.ReadStructBeginAsync(cancellationToken);
        while (true)
        {
          field = await iprot.ReadFieldBeginAsync(cancellationToken);
          if (field.Type == TType.Stop)
          {
            break;
          }

          switch (field.ID)
          {
            case 1:
              if (field.Type == TType.I64)
              {
                Value = await iprot.ReadI64Async(cancellationToken);
              }
              else
              {
                await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
              }
              break;
            default:
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
              break;
          }

          await iprot.ReadFieldEndAsync(cancellationToken);
        }

        await iprot.ReadStructEndAsync(cancellationToken);
      }
      finally
      {
        iprot.DecrementRecursionDepth();
      }
    }

    public async global::System.Threading.Tasks.Task WriteAsync(TProtocol oprot, CancellationToken cancellationToken)
    {
      oprot.IncrementRecursionDepth();
      try
      {
        var tmp104 = new TStruct("TI64Value");
        await oprot.WriteStructBeginAsync(tmp104, cancellationToken);
        var tmp105 = new TField();
        if (__isset.@value)
        {
          tmp105.Name = "value";
          tmp105.Type = TType.I64;
          tmp105.ID = 1;
          await oprot.WriteFieldBeginAsync(tmp105, cancellationToken);
          await oprot.WriteI64Async(Value, cancellationToken);
          await oprot.WriteFieldEndAsync(cancellationToken);
        }
        await oprot.WriteFieldStopAsync(cancellationToken);
        await oprot.WriteStructEndAsync(cancellationToken);
      }
      finally
      {
        oprot.DecrementRecursionDepth();
      }
    }

    public override bool Equals(object that)
    {
      if (!(that is TI64Value other)) return false;
      if (ReferenceEquals(this, other)) return true;
      return ((__isset.@value == other.__isset.@value) && ((!__isset.@value) || (global::System.Object.Equals(Value, other.Value))));
    }

    public override int GetHashCode() {
      int hashcode = 157;
      unchecked {
        if (__isset.@value)
        {
          hashcode = (hashcode * 397) + Value.GetHashCode();
        }
      }
      return hashcode;
    }

    public override string ToString()
    {
      var tmp106 = new StringBuilder("TI64Value(");
      int tmp107 = 0;
      if (__isset.@value)
      {
        if (0 < tmp107++) { tmp106.Append(", "); }
        tmp106.Append("Value: ");
        Value.ToString(tmp106);
      }
      tmp106.Append(')');
      return tmp106.ToString();
    }
  }

}
