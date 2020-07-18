[@react.component]
let make = () => {
  <div id="dashboard">
    <h1> "Dashboard"->React.string </h1>
    <table>
      <tr>
        <th> "id"->React.string </th>
        <th> "label"->React.string </th>
        <th> "last backed up"->React.string </th>
      </tr>
      <tr>
        <td> "1"->React.string </td>
        <td> "my daily backup"->React.string </td>
        <td> "just now"->React.string </td>
      </tr>
    </table>
  </div>;
};
