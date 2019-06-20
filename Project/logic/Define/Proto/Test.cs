// <auto-generated>
//     Generated by the protocol buffer compiler.  DO NOT EDIT!
//     source: test.proto
// </auto-generated>
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
/// <summary>Holder for reflection information generated from test.proto</summary>
public static partial class TestReflection {

  #region Descriptor
  /// <summary>File descriptor for test.proto</summary>
  public static pbr::FileDescriptor Descriptor {
    get { return descriptor; }
  }
  private static pbr::FileDescriptor descriptor;

  static TestReflection() {
    byte[] descriptorData = global::System.Convert.FromBase64String(
        string.Concat(
          "Cgp0ZXN0LnByb3RvIjAKDVNlYXJjaFJlcXVlc3QSDQoFcXVlcnkYASABKAkS",
          "EAoIcGFnZV9udW0YAiABKAViBnByb3RvMw=="));
    descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
        new pbr::FileDescriptor[] { },
        new pbr::GeneratedClrTypeInfo(null, new pbr::GeneratedClrTypeInfo[] {
          new pbr::GeneratedClrTypeInfo(typeof(global::SearchRequest), global::SearchRequest.Parser, new[]{ "Query", "PageNum" }, null, null, null)
        }));
  }
  #endregion

}
#region Messages
public sealed partial class SearchRequest : pb::IMessage<SearchRequest> {
  private static readonly pb::MessageParser<SearchRequest> _parser = new pb::MessageParser<SearchRequest>(() => new SearchRequest());
  private pb::UnknownFieldSet _unknownFields;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pb::MessageParser<SearchRequest> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::TestReflection.Descriptor.MessageTypes[0]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public SearchRequest() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public SearchRequest(SearchRequest other) : this() {
    query_ = other.query_;
    pageNum_ = other.pageNum_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public SearchRequest Clone() {
    return new SearchRequest(this);
  }

  /// <summary>Field number for the "query" field.</summary>
  public const int QueryFieldNumber = 1;
  private string query_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string Query {
    get { return query_; }
    set {
      query_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  /// <summary>Field number for the "page_num" field.</summary>
  public const int PageNumFieldNumber = 2;
  private int pageNum_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int PageNum {
    get { return pageNum_; }
    set {
      pageNum_ = value;
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override bool Equals(object other) {
    return Equals(other as SearchRequest);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool Equals(SearchRequest other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (Query != other.Query) return false;
    if (PageNum != other.PageNum) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override int GetHashCode() {
    int hash = 1;
    if (Query.Length != 0) hash ^= Query.GetHashCode();
    if (PageNum != 0) hash ^= PageNum.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void WriteTo(pb::CodedOutputStream output) {
    if (Query.Length != 0) {
      output.WriteRawTag(10);
      output.WriteString(Query);
    }
    if (PageNum != 0) {
      output.WriteRawTag(16);
      output.WriteInt32(PageNum);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int CalculateSize() {
    int size = 0;
    if (Query.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Query);
    }
    if (PageNum != 0) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(PageNum);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(SearchRequest other) {
    if (other == null) {
      return;
    }
    if (other.Query.Length != 0) {
      Query = other.Query;
    }
    if (other.PageNum != 0) {
      PageNum = other.PageNum;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(pb::CodedInputStream input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 10: {
          Query = input.ReadString();
          break;
        }
        case 16: {
          PageNum = input.ReadInt32();
          break;
        }
      }
    }
  }

}

#endregion


#endregion Designer generated code
