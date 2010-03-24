/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package com.sun.star.report;

import com.sun.star.beans.PropertyVetoException;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.IndexOutOfBoundsException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.sdbc.XConnection;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdb.XCompletedExecution;
import com.sun.star.sdb.XParametersSupplier;
import com.sun.star.sdb.XQueriesSupplier;
import com.sun.star.sdb.XResultSetAccess;
import com.sun.star.sdb.XSingleSelectQueryComposer;
import com.sun.star.sdb.tools.XConnectionTools;
import com.sun.star.sdbc.SQLException;
import com.sun.star.sdbc.XParameters;
import com.sun.star.sdbc.XPreparedStatement;
import com.sun.star.uno.Exception;
import java.util.HashMap;
import java.util.Map;

import com.sun.star.sdbc.XRowSet;
import com.sun.star.sdbcx.XColumnsSupplier;
import com.sun.star.sdbcx.XTablesSupplier;
import com.sun.star.task.XInteractionHandler;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import java.lang.reflect.Method;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Very primitive implementation, just to show how this could be used ...
 * 
 */
public class SDBCReportDataFactory implements DataSourceFactory
{
    private static final String ESCAPEPROCESSING = "EscapeProcessing";

    private class RowSetProperties
    {

        final Boolean escapeProcessing;
        final int commandType;
        final Integer maxRows;
        final String command;
        final String filter;

        public RowSetProperties(final Boolean escapeProcessing, final int commandType, final String command, final String filter, final Integer maxRows)
        {
            this.escapeProcessing = escapeProcessing;
            this.commandType = commandType;
            this.command = command;
            this.filter = filter;
            this.maxRows = maxRows;
        }


        public boolean equals(Object obj)
        {
            if (obj == null)
            {
                return false;
            }
            if (getClass() != obj.getClass())
            {
                return false;
            }
            final RowSetProperties other = (RowSetProperties) obj;
            if (this.escapeProcessing != other.escapeProcessing && (this.escapeProcessing == null || !this.escapeProcessing.equals(other.escapeProcessing)))
            {
                return false;
            }
            if (this.commandType != other.commandType)
            {
                return false;
            }
            if (this.maxRows != other.maxRows && (this.maxRows == null || !this.maxRows.equals(other.maxRows)))
            {
                return false;
            }
            if ((this.command == null) ? (other.command != null) : !this.command.equals(other.command))
            {
                return false;
            }
            if ((this.filter == null) ? (other.filter != null) : !this.filter.equals(other.filter))
            {
                return false;
            }
            return true;
        }

        public int hashCode()
        {
            int hash = 3;
            hash = 59 * hash + (this.escapeProcessing != null ? this.escapeProcessing.hashCode() : 0);
            hash = 59 * hash + this.commandType;
            hash = 59 * hash + (this.maxRows != null ? this.maxRows.hashCode() : 0);
            hash = 59 * hash + (this.command != null ? this.command.hashCode() : 0);
            hash = 59 * hash + (this.filter != null ? this.filter.hashCode() : 0);
            return hash;
        }
    }
    class ParameterDefinition
    {
        int parameterCount = 0;
        private ArrayList parameterIndex = new ArrayList();
    }
    private static final Log LOGGER = LogFactory.getLog(SDBCReportDataFactory.class);
    public static final String COMMAND_TYPE = "command-type";
    public static final String ESCAPE_PROCESSING = "escape-processing";
    public static final String GROUP_EXPRESSIONS = "group-expressions";
    public static final String MASTER_VALUES = "master-values";
    public static final String MASTER_COLUMNS = "master-columns";
    public static final String DETAIL_COLUMNS = "detail-columns";
    public static final String UNO_FILTER = "Filter";
    private static final String APPLY_FILTER = "ApplyFilter";
    private static final String UNO_COMMAND = "Command";
    private static final String UNO_ORDER = "Order";
    private static final String UNO_APPLY_FILTER = "ApplyFilter";
    private static final String UNO_COMMAND_TYPE = "CommandType";
    private final XConnection connection;
    private final XComponentContext m_cmpCtx;
    private static final int FAILED = 0;
    private static final int DONE = 1;
    private static final int RETRIEVE_COLUMNS = 2;
    private static final int RETRIEVE_OBJECT = 3;
    private static final int HANDLE_QUERY = 4;
    private static final int HANDLE_TABLE = 5;
    private static final int HANDLE_SQL = 6;
    private final Map rowSetProperties = new HashMap();
    private final Map parameterMap = new HashMap();
    private boolean rowSetCreated = false;

    public SDBCReportDataFactory(final XComponentContext cmpCtx, final XConnection connection)
    {
        this.connection = connection;
        m_cmpCtx = cmpCtx;
    }

    public DataSource queryData(final String command, final Map parameters) throws DataSourceException
    {
        try
        {
            if (command == null)
            {
                return new SDBCReportData(null);
            }
            int commandType = CommandType.COMMAND;
            final String commandTypeValue = (String) parameters.get(COMMAND_TYPE);
            if (commandTypeValue != null)
            {
                if ("query".equals(commandTypeValue))
                {
                    commandType = CommandType.QUERY;
                }
                else if ("table".equals(commandTypeValue))
                {
                    commandType = CommandType.TABLE;
                }
                else
                {
                    commandType = CommandType.COMMAND;
                }
            }
            final Boolean escapeProcessing = (Boolean) parameters.get(ESCAPE_PROCESSING);
            final String filter = (String) parameters.get(UNO_FILTER);
            final Integer maxRows = (Integer) parameters.get("MaxRows");
            RowSetProperties rowSetProps = new RowSetProperties(escapeProcessing, commandType, command, filter, maxRows);

            final Object[] p = createRowSet(rowSetProps,parameters);
            final XRowSet rowSet = (XRowSet)p[0];
            
            if (command.length() != 0 )
            {
                final ParameterDefinition paramDef = (ParameterDefinition)p[1];
                fillParameter(parameters, rowSet,paramDef);
                rowSetCreated = rowSetCreated && ( maxRows == null || maxRows.intValue() == 0);

                final XCompletedExecution execute = (XCompletedExecution) UnoRuntime.queryInterface(XCompletedExecution.class, rowSet);
                if (rowSetCreated && execute != null && paramDef.parameterCount > 0)
                {
                    final XInteractionHandler handler = (XInteractionHandler) UnoRuntime.queryInterface(XInteractionHandler.class, m_cmpCtx.getServiceManager().createInstanceWithContext("com.sun.star.sdb.InteractionHandler", m_cmpCtx));
                    execute.executeWithCompletion(handler);
                }
                else
                {
                    rowSet.execute();
                }
            }

            rowSetCreated = false;
            return new SDBCReportData(rowSet);
        }
        catch (Exception ex)
        {
            rowSetCreated = false;
            throw new DataSourceException(ex.getMessage(), ex);
        }
    }

    private String getOrderStatement(final int commandType, final String command, final List groupExpressions)
    {
        final StringBuffer order = new StringBuffer();
        final int count = groupExpressions.size();
        if (count != 0)
        {
            try
            {
                final String quote = connection.getMetaData().getIdentifierQuoteString();
                final XComponent[] hold = new XComponent[1];
                final XNameAccess columns = getFieldsByCommandDescriptor(commandType, command, hold);

                for (int i = 0; i < count; i++)
                {
                    final Object[] pair = (Object[]) groupExpressions.get(i);
                    String expression = (String) pair[0];

                    if (columns.hasByName(expression))
                    {
                        expression = quote + expression + quote;
                    }
                    expression = expression.trim(); // Trim away white spaces

                    if (expression.length() > 0)
                    {
                        order.append(expression);
                        if (order.length() > 0)
                        {
                            order.append(' ');
                        }
                        final String sorting = (String) pair[1];
                        if (sorting == null || sorting.equals(OfficeToken.FALSE))
                        {
                            order.append("DESC");
                        }
                        if ((i + 1) < count)
                        {
                            order.append(", ");
                        }
                    }
                }
            }
            catch (SQLException ex)
            {
                LOGGER.error("ReportProcessing failed", ex);
            }
        }
        return order.toString();
    }

    private XNameAccess getFieldsByCommandDescriptor(final int commandType, final String command, final XComponent[] out) throws SQLException
    {
        final Class[] parameter = new Class[3];
        parameter[0] = Integer.class;
        parameter[1] = String.class;
        parameter[2] = out.getClass();
        final XConnectionTools tools = (XConnectionTools) UnoRuntime.queryInterface(XConnectionTools.class, connection);
        try
        {
            tools.getClass().getMethod("getFieldsByCommandDescriptor", parameter);
            return tools.getFieldsByCommandDescriptor(commandType, command, out);
        }
        catch (NoSuchMethodException ex)
        {
        }

        XNameAccess xFields = null;
        // some kind of state machine to ease the sharing of code
        int eState = FAILED;
        switch (commandType)
        {
            case CommandType.TABLE:
                eState = HANDLE_TABLE;
                break;
            case CommandType.QUERY:
                eState = HANDLE_QUERY;
                break;
            case CommandType.COMMAND:
                eState = HANDLE_SQL;
                break;
        }

        // needed in various states:
        XNameAccess xObjectCollection = null;
        XColumnsSupplier xSupplyColumns = null;

        try
        {
            // go!
            while ((DONE != eState) && (FAILED != eState))
            {
                switch (eState)
                {
                    case HANDLE_TABLE:
                    {
                        // initial state for handling the tables

                        // get the table objects
                        final XTablesSupplier xSupplyTables = (XTablesSupplier) UnoRuntime.queryInterface(XTablesSupplier.class, connection);
                        if (xSupplyTables != null)
                        {
                            xObjectCollection = xSupplyTables.getTables();
                            // if something went wrong 'til here, then this will be handled in the next state

                            // next state: get the object
                            }
                        eState = RETRIEVE_OBJECT;
                    }
                    break;

                    case HANDLE_QUERY:
                    {
                        // initial state for handling the tables

                        // get the table objects
                        final XQueriesSupplier xSupplyQueries = (XQueriesSupplier) UnoRuntime.queryInterface(XQueriesSupplier.class, connection);
                        if (xSupplyQueries != null)
                        {
                            xObjectCollection = xSupplyQueries.getQueries();
                            // if something went wrong 'til here, then this will be handled in the next state

                            // next state: get the object
                            }
                        eState = RETRIEVE_OBJECT;
                    }
                    break;

                    case RETRIEVE_OBJECT:
                        // here we should have an object (aka query or table) collection, and are going
                        // to retrieve the desired object

                        // next state: default to FAILED
                        eState = FAILED;

                        if (xObjectCollection != null && xObjectCollection.hasByName(command))
                        {
                            xSupplyColumns = (XColumnsSupplier) UnoRuntime.queryInterface(XColumnsSupplier.class, xObjectCollection.getByName(command));

                            // next: go for the columns
                            eState = RETRIEVE_COLUMNS;
                        }
                        break;

                    case RETRIEVE_COLUMNS:
                        // next state: default to FAILED
                        eState = FAILED;

                        if (xSupplyColumns != null)
                        {
                            xFields = xSupplyColumns.getColumns();
                            // that's it
                            eState = DONE;
                        }
                        break;

                    case HANDLE_SQL:
                    {
                        String sStatementToExecute = command;

                        // well, the main problem here is to handle statements which contain a parameter
                        // If we would simply execute a parametrized statement, then this will fail because
                        // we cannot supply any parameter values.
                        // Thus, we try to analyze the statement, and to append a WHERE 0=1 filter criterion
                        // This should cause every driver to not really execute the statement, but to return
                        // an empty result set with the proper structure. We then can use this result set
                        // to retrieve the columns.

                        try
                        {
                            final XMultiServiceFactory xComposerFac = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, connection);

                            if (xComposerFac != null)
                            {
                                final XSingleSelectQueryComposer xComposer = (XSingleSelectQueryComposer) UnoRuntime.queryInterface(XSingleSelectQueryComposer.class, xComposerFac.createInstance("com.sun.star.sdb.SingleSelectQueryComposer"));
                                if (xComposer != null)
                                {
                                    xComposer.setQuery(sStatementToExecute);

                                    // Now set the filter to a dummy restriction which will result in an empty
                                    // result set.
                                    xComposer.setFilter("0=1");

                                    sStatementToExecute = xComposer.getQuery();
                                }
                            }
                        }
                        catch (com.sun.star.uno.Exception ex)
                        {
                            // silent this error, this was just a try. If we're here, we did not change sStatementToExecute,
                            // so it will still be _rCommand, which then will be executed without being touched
                            }

                        // now execute
                        final XPreparedStatement xStatement = connection.prepareStatement(sStatementToExecute);
                        // transfer ownership of this temporary object to the caller
                        out[0] = (XComponent) UnoRuntime.queryInterface(XComponent.class, xStatement);

                        // set the "MaxRows" to 0. This is just in case our attempt to append a 0=1 filter
                        // failed - in this case, the MaxRows restriction should at least ensure that there
                        // is no data returned (which would be potentially expensive)
                        final XPropertySet xStatementProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xStatement);
                        try
                        {
                            if (xStatementProps != null)
                            {
                                xStatementProps.setPropertyValue("MaxRows", Integer.valueOf(0));
                            }
                        }
                        catch (com.sun.star.uno.Exception ex)
                        {
                            // oh damn. Not much of a chance to recover, we will no retrieve the complete
                            // full blown result set
                            }

                        xSupplyColumns = (XColumnsSupplier) UnoRuntime.queryInterface(XColumnsSupplier.class, xStatement.executeQuery());
                        // this should have given us a result set which does not contain any data, but
                        // the structural information we need

                        // so the next state is to get the columns
                        eState = RETRIEVE_COLUMNS;
                    }
                    break;
                    default:
                        eState = FAILED;
                }
            }
        }
        catch (com.sun.star.uno.Exception ex)
        {
        }
        return xFields;
    }

    private XSingleSelectQueryComposer getComposer(final XConnectionTools tools,
            final String command,
            final int commandType)
    {
        final Class[] parameter = new Class[2];
        parameter[0] = int.class;
        parameter[1] = String.class;
        try
        {
            final Object[] param = new Object[2];
            param[0] = commandType;
            param[1] = command;
            final Method call = tools.getClass().getMethod("getComposer", parameter);
            return (XSingleSelectQueryComposer) call.invoke(tools, param);
        }
        catch (NoSuchMethodException ex)
        {
        }
        catch (IllegalAccessException ex)
        {
            // should not happen
            // assert False
        }
        catch (java.lang.reflect.InvocationTargetException ex)
        {
            // should not happen
            // assert False
        }
        try
        {
            final XMultiServiceFactory factory = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, connection);
            final XSingleSelectQueryComposer out = (XSingleSelectQueryComposer) UnoRuntime.queryInterface(XSingleSelectQueryComposer.class, factory.createInstance("com.sun.star.sdb.SingleSelectQueryAnalyzer"));
            final String quote = connection.getMetaData().getIdentifierQuoteString();
            String statement = command;
            switch (commandType)
            {
                case CommandType.TABLE:
                    statement = "SELECT * FROM " + quote + command + quote;
                    break;
                case CommandType.QUERY:
                {
                    final XQueriesSupplier xSupplyQueries = (XQueriesSupplier) UnoRuntime.queryInterface(XQueriesSupplier.class, connection);
                    final XNameAccess queries = xSupplyQueries.getQueries();
                    if (queries.hasByName(command))
                    {
                        final XPropertySet prop = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, queries.getByName(command));
                        final Boolean escape = (Boolean) prop.getPropertyValue(ESCAPEPROCESSING);
                        if (escape.booleanValue())
                        {
                            statement = (String) prop.getPropertyValue(UNO_COMMAND);
                            final XSingleSelectQueryComposer composer = getComposer(tools, statement, CommandType.COMMAND);
                            if (composer != null)
                            {
                                final String order = (String) prop.getPropertyValue(UNO_ORDER);
                                if (order != null && order.length() != 0)
                                {
                                    composer.setOrder(order);
                                }
                                final Boolean applyFilter = (Boolean) prop.getPropertyValue(UNO_APPLY_FILTER);
                                if (applyFilter.booleanValue())
                                {
                                    final String filter = (String) prop.getPropertyValue(UNO_FILTER);
                                    if (filter != null && filter.length() != 0)
                                    {
                                        composer.setFilter(filter);
                                    }
                                }
                                statement = composer.getQuery();
                            }
                        }
                    }
                    break;
                }
                case CommandType.COMMAND:
                    statement = command;
                    break;
            }
            out.setElementaryQuery(statement);
            return out;
        }
        catch (Exception e)
        {
        }
        return null;
    }

    private void fillParameter(final Map parameters,
            final XRowSet rowSet,final ParameterDefinition paramDef)
            throws SQLException,
            UnknownPropertyException,
            PropertyVetoException,
            IllegalArgumentException,
            WrappedTargetException
    {
        final ArrayList masterValues = (ArrayList) parameters.get(MASTER_VALUES);
        if (masterValues != null && !masterValues.isEmpty())
        {
            final XParameters para = (XParameters) UnoRuntime.queryInterface(XParameters.class, rowSet);

            for (int i = 0;
                    i < masterValues.size();
                    i++)
            {
                Object object = masterValues.get(i);
                if (object instanceof BigDecimal)
                {
                    object = ((BigDecimal) object).toString();
                }
                final Integer pos = (Integer)paramDef.parameterIndex.get(i);
                para.setObject(pos + 1, object);
            }
        }
    }

    private final Object[] createRowSet(final RowSetProperties rowSetProps,final Map parameters)
            throws Exception
    {
        final ArrayList detailColumns = (ArrayList) parameters.get(DETAIL_COLUMNS);
        if (rowSetProperties.containsKey(rowSetProps) && detailColumns != null && !detailColumns.isEmpty() )
        {
            return new Object[]{ rowSetProperties.get(rowSetProps),parameterMap.get(rowSetProps)};
        }

        rowSetCreated = true;
        final XRowSet rowSet = (XRowSet) UnoRuntime.queryInterface(XRowSet.class, m_cmpCtx.getServiceManager().createInstanceWithContext("com.sun.star.sdb.RowSet", m_cmpCtx));
        final XPropertySet rowSetProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, rowSet);

        rowSetProp.setPropertyValue("ActiveConnection", connection);
        rowSetProp.setPropertyValue(ESCAPEPROCESSING, rowSetProps.escapeProcessing);
        rowSetProp.setPropertyValue(UNO_COMMAND_TYPE, Integer.valueOf(rowSetProps.commandType));
        rowSetProp.setPropertyValue(UNO_COMMAND, rowSetProps.command);

        if (rowSetProps.filter != null)
        {
            rowSetProp.setPropertyValue("Filter", rowSetProps.filter);
            rowSetProp.setPropertyValue(APPLY_FILTER, Boolean.valueOf(rowSetProps.filter.length() != 0));
        }
        else
        {
            rowSetProp.setPropertyValue(APPLY_FILTER, Boolean.FALSE);
        }

        if (rowSetProps.maxRows != null)
        {
            rowSetProp.setPropertyValue("MaxRows", rowSetProps.maxRows);
        }

        final XConnectionTools tools = (XConnectionTools) UnoRuntime.queryInterface(XConnectionTools.class, connection);
        fillOrderStatement(rowSetProps.command, rowSetProps.commandType, parameters, tools, rowSetProp);
        final ParameterDefinition paramDef = createParameter(parameters, tools, rowSetProps, rowSet);

        rowSetProperties.put(rowSetProps, rowSet);
        parameterMap.put(rowSetProps, paramDef);

        return new Object[]{rowSet,paramDef};
    }

    private ParameterDefinition createParameter(final Map parameters,
            final XConnectionTools tools,
            RowSetProperties rowSetProps, final XRowSet rowSet)
            throws SQLException,
            UnknownPropertyException,
            PropertyVetoException,
            IllegalArgumentException,
            WrappedTargetException
    {
        final ParameterDefinition paramDef = new ParameterDefinition();
        final XSingleSelectQueryComposer composer = getComposer(tools, rowSetProps.command, rowSetProps.commandType);
        if (composer != null)
        {
            final XPropertySet rowSetProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, rowSet);
            if (((Boolean) rowSetProp.getPropertyValue(APPLY_FILTER)).booleanValue())
            {
                composer.setFilter((String) rowSetProp.getPropertyValue("Filter"));
            }
            // get old parameter count
            final ArrayList detailColumns = (ArrayList) parameters.get(DETAIL_COLUMNS);
            final ArrayList handledColumns = new ArrayList();
            final XParametersSupplier paraSup = (XParametersSupplier) UnoRuntime.queryInterface(XParametersSupplier.class, composer);
            if (paraSup != null)
            {
                final XIndexAccess params = paraSup.getParameters();
                if (params != null)
                {
                    final int oldParameterCount = params.getCount();
                    paramDef.parameterCount = oldParameterCount;
                    if ( detailColumns != null )
                    {
                        for (int i = 0; i < oldParameterCount; i++)
                        {
                            try
                            {
                                final XPropertySet parameter = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, params.getByIndex(i));
                                if (parameter != null)
                                {
                                    final String name = (String) parameter.getPropertyValue("Name");
                                    for (int j = 0; j < detailColumns.size(); j++)
                                    {
                                        if ( name.equals(detailColumns.get(j) ) )
                                        {
                                            handledColumns.add(name);
                                            paramDef.parameterIndex.add(Integer.valueOf(i));
                                            --paramDef.parameterCount;
                                            break;
                                        }
                                    }
                                }
                            }
                            catch (IndexOutOfBoundsException ex)
                            {
                                Logger.getLogger(SDBCReportDataFactory.class.getName()).log(Level.SEVERE, null, ex);
                            }
                        }
                    }
                }
            }
            final ArrayList masterValues = (ArrayList) parameters.get(MASTER_VALUES);
            if (masterValues != null && !masterValues.isEmpty() && paramDef.parameterIndex.size() != detailColumns.size() )
            {
                // Vector masterColumns = (Vector) parameters.get("master-columns");
                
                // create the new filter
                final String quote = connection.getMetaData().getIdentifierQuoteString();
                final StringBuffer oldFilter = new StringBuffer();
                oldFilter.append(composer.getFilter());
                if (oldFilter.length() != 0)
                {
                    oldFilter.append(" AND ");
                }
                int newParamterCounter = 1;
                for (final Iterator it = detailColumns.iterator(); it.hasNext();
                        ++newParamterCounter)
                {
                    final String detail = (String) it.next();
                    if ( !handledColumns.contains(detail) )
                    {
                        //String master = (String) masterIt.next();
                        oldFilter.append(quote);
                        oldFilter.append(detail);
                        oldFilter.append(quote);
                        oldFilter.append(" = :link_");
                        oldFilter.append(newParamterCounter);
                        if (it.hasNext())
                        {
                            oldFilter.append(" AND ");
                        }
                        paramDef.parameterIndex.add(Integer.valueOf(newParamterCounter + paramDef.parameterCount - 1));
                    }
                }

                composer.setFilter(oldFilter.toString());

                final String sQuery = composer.getQuery();
                rowSetProp.setPropertyValue(UNO_COMMAND, sQuery);
                rowSetProp.setPropertyValue(UNO_COMMAND_TYPE,Integer.valueOf(CommandType.COMMAND));
            }
        }
        return paramDef;
    }

    void fillOrderStatement(final String command,
            final int commandType, final Map parameters,
            final XConnectionTools tools,
            final XPropertySet rowSetProp)
            throws SQLException,
            UnknownPropertyException,
            PropertyVetoException,
            IllegalArgumentException,
            WrappedTargetException,
            NoSuchElementException
    {
        final StringBuffer order = new StringBuffer(getOrderStatement(commandType, command, (ArrayList) parameters.get(GROUP_EXPRESSIONS)));
        if (order.length() > 0 && commandType != CommandType.TABLE)
        {
            String statement = command;
            final XSingleSelectQueryComposer composer = getComposer(tools, command, commandType);
            if (composer != null)
            {
                statement = composer.getQuery();
                composer.setQuery(statement);
                final String sOldOrder = composer.getOrder();
                if (sOldOrder.length() > 0)
                {
                    order.append(',');
                    order.append(sOldOrder);
                    composer.setOrder("");
                    statement = composer.getQuery();
                }
            }
            else
            {
                if (commandType == CommandType.QUERY)
                {
                    final XQueriesSupplier xSupplyQueries = (XQueriesSupplier) UnoRuntime.queryInterface(XQueriesSupplier.class, connection);
                    final XNameAccess queries = xSupplyQueries.getQueries();
                    if (queries.hasByName(command))
                    {
                        final XPropertySet prop = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, queries.getByName(command));
                        final Boolean escape = (Boolean) prop.getPropertyValue(ESCAPEPROCESSING);
                        rowSetProp.setPropertyValue( ESCAPEPROCESSING, escape);
                        final String queryCommand = (String) prop.getPropertyValue(UNO_COMMAND);
                        statement = "SELECT * FROM (" + queryCommand + ")";
                    }

                }
                else
                {
                    statement = "SELECT * FROM (" + command + ")";
                }
            }
            rowSetProp.setPropertyValue(UNO_COMMAND, statement);
            rowSetProp.setPropertyValue(UNO_COMMAND_TYPE, Integer.valueOf(CommandType.COMMAND));
        }
        rowSetProp.setPropertyValue("Order", order.toString());
    }
}

